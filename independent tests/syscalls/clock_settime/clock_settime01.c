#include "incl.h"
#define DELTA_SEC 10
#define DELTA_US (long long) (DELTA_SEC * 1000000)
#define DELTA_EPS (long long) (DELTA_US * 0.1)

struct tst_ts *begin, *change, *end;

struct time64_variants variants[] = {
	{ .clock_gettime = libc_clock_gettime, .clock_settime = libc_clock_settime, .ts_type = TST_LIBC_TIMESPEC, .desc = "vDSO or syscall with libc spec"},
#if (__NR_clock_settime != __LTP__NR_INVALID_SYSCALL)
	{ .clock_gettime = sys_clock_gettime, .clock_settime = sys_clock_settime, .ts_type = TST_KERN_OLD_TIMESPEC, .desc = "syscall with old kernel spec"},
#endif
#if (__NR_clock_settime64 != __LTP__NR_INVALID_SYSCALL)
	{ .clock_gettime = sys_clock_gettime64, .clock_settime = sys_clock_settime64, .ts_type = TST_KERN_TIMESPEC, .desc = "syscall time64 with kernel spec"},
#endif
};

void setup(void)
{
	begin->type = change->type = end->type = variants[tst_variant].ts_type;
	tst_res(TINFO, "Testing variant: %s", variants[tst_variant].desc);
}

void do_clock_gettime(struct time64_variants *tv, struct tst_ts *ts)
{
	int ret;
	ret = tv->clock_gettime(CLOCK_REALTIME, tst_ts_get(ts));
	if (ret == -1)
		tst_brk(TBROK | TERRNO, "clock_settime(CLOCK_REALTIME) failed");
}

int  verify_clock_settime(
{
	struct time64_variants *tv = &variants[tst_variant];
	long long elapsed;
	do_clock_gettime(tv, begin);
tv->clock_settime(CLOCK_REALTIME, tst_ts_get(change));
	if (TST_RET == -1) {
		tst_res(TFAIL | TTERRNO, "clock_settime(2) failed for clock %s",
			tst_clock_name(CLOCK_REALTIME));
		return;
	}
	do_clock_gettime(tv, end);
	elapsed = tst_ts_diff_us(*end, *begin);
	if (elapsed >= DELTA_US && elapsed < (DELTA_US + DELTA_EPS))
		tst_res(TPASS, "clock_settime(2): was able to advance time");
	else
		tst_res(TFAIL, "clock_settime(2): could not advance time");
	do_clock_gettime(tv, begin);
tv->clock_settime(CLOCK_REALTIME, tst_ts_get(change));
	if (TST_RET == -1) {
		tst_res(TFAIL | TTERRNO, "clock_settime(2) failed for clock %s",
			tst_clock_name(CLOCK_REALTIME));
		return;
	}
	do_clock_gettime(tv, end);
	elapsed = tst_ts_diff_us(*end, *begin);
	if (~(elapsed) <= DELTA_US && ~(elapsed) > (DELTA_US - DELTA_EPS))
		tst_res(TPASS, "clock_settime(2): was able to recede time");
	else
		tst_res(TFAIL, "clock_settime(2): could not recede time");
}

void main(){
	setup();
	cleanup();
}
