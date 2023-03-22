#include "incl.h"
#define _GNU_SOURCE

struct tcase {
	int clk_id;
	int clk_off;
	int off;
} tcases[] = {
	{CLOCK_MONOTONIC, CLOCK_MONOTONIC, 10},
	{CLOCK_BOOTTIME, CLOCK_BOOTTIME, 10},
	{CLOCK_MONOTONIC, CLOCK_MONOTONIC, -10},
	{CLOCK_BOOTTIME, CLOCK_BOOTTIME, -10},
	{CLOCK_MONOTONIC_RAW, CLOCK_MONOTONIC, 100},
	{CLOCK_MONOTONIC_COARSE, CLOCK_MONOTONIC, 100},
};

struct tst_ts now, then, parent_then;

int parent_ns;

long long delta = 10;

struct time64_variants variants[] = {
	{ .clock_gettime = libc_clock_gettime, .ts_type = TST_LIBC_TIMESPEC, .desc = "vDSO or syscall with libc spec"},
#if (__NR_clock_gettime != __LTP__NR_INVALID_SYSCALL)
	{ .clock_gettime = sys_clock_gettime, .ts_type = TST_KERN_OLD_TIMESPEC, .desc = "syscall with old kernel spec"},
#endif
#if (__NR_clock_gettime64 != __LTP__NR_INVALID_SYSCALL)
	{ .clock_gettime = sys_clock_gettime64, .ts_type = TST_KERN_TIMESPEC, .desc = "syscall time64 with kernel spec"},
#endif
};

void child(struct time64_variants *tv, struct tcase *tc)
{
	long long diff;
	if (tv->clock_gettime(tc->clk_id, tst_ts_get(&then))) {
		tst_res(TFAIL | TERRNO, "clock_gettime(%s) failed",
			tst_clock_name(tc->clk_id));
		return;
	}
	setns(parent_ns, CLONE_NEWTIME);
	if (tv->clock_gettime(tc->clk_id, tst_ts_get(&parent_then))) {
		tst_res(TFAIL | TERRNO, "clock_gettime(%s) failed",
			tst_clock_name(tc->clk_id));
		return;
	}
	diff = tst_ts_diff_ms(then, now);
	if (diff - tc->off * 1000 > delta) {
		tst_res(TFAIL, "Wrong offset (%s) read %llims",
		        tst_clock_name(tc->clk_id), diff);
	} else {
		tst_res(TPASS, "Offset (%s) is correct %llims",
		        tst_clock_name(tc->clk_id), diff);
	}
	diff = tst_ts_diff_ms(parent_then, now);
	if (diff > delta) {
		tst_res(TFAIL, "Wrong offset (%s) read %llims",
		        tst_clock_name(tc->clk_id), diff);
	} else {
		tst_res(TPASS, "Offset (%s) is correct %llims",
		        tst_clock_name(tc->clk_id), diff);
	}
}

int  verify_ns_clock(unsigned int n)
{
	struct time64_variants *tv = &variants[tst_variant];
	struct tcase *tc = &tcases[n];
	unshare(CLONE_NEWTIME);
	fprintf("/proc/self/timens_offsets", "%d %d 0",
	                 tc->clk_off, tc->off);
	if (tv->clock_gettime(tc->clk_id, tst_ts_get(&now))) {
		tst_res(TFAIL | TERRNO, "%d clock_gettime(%s) failed",
			__LINE__, tst_clock_name(tc->clk_id));
		return;
	}
	if (!fork())
		child(tv, tc);
}

void setup(void)
{
	struct time64_variants *tv = &variants[tst_variant];
	if (tst_is_virt(VIRT_ANY)) {
		tst_res(TINFO, "Running in a VM, multiply the delta by 10.");
		delta *= 10;
	}
	now.type = then.type = parent_then.type = tv->ts_type;
	tst_res(TINFO, "Testing variant: %s", variants[tst_variant].desc);
	parent_ns = open("/proc/self/ns/time_for_children", O_RDONLY);
}

void cleanup(void)
{
	close(parent_ns);
}

void main(){
	setup();
	cleanup();
}
