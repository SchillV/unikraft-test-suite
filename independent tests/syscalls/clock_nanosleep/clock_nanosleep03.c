#include "incl.h"
#define OFFSET_S 10
#define SLEEP_US 100000

struct time64_variants variants[] = {
	{ .clock_gettime = libc_clock_gettime, .clock_nanosleep = libc_clock_nanosleep, .ts_type = TST_LIBC_TIMESPEC, .desc = "vDSO or syscall with libc spec"},
#if (__NR_clock_nanosleep != __LTP__NR_INVALID_SYSCALL)
	{ .clock_gettime = sys_clock_gettime, .clock_nanosleep = sys_clock_nanosleep, .ts_type = TST_KERN_OLD_TIMESPEC, .desc = "syscall with old kernel spec"},
#endif
#if (__NR_clock_nanosleep_time64 != __LTP__NR_INVALID_SYSCALL)
	{ .clock_gettime = sys_clock_gettime64, .clock_nanosleep = sys_clock_nanosleep64, .ts_type = TST_KERN_TIMESPEC, .desc = "syscall time64 with kernel spec"},
#endif
};

void do_clock_gettime(struct time64_variants *tv, struct tst_ts *ts)
{
	int ret;
	ret = tv->clock_gettime(CLOCK_MONOTONIC, tst_ts_get(ts));
	if (ret == -1)
		tst_brk(TBROK | TERRNO, "clock_settime(CLOCK_MONOTONIC) failed");
}

int  verify_clock_nanosleep(
{
	struct time64_variants *tv = &variants[tst_variant];
	struct tst_ts start, end, sleep_abs;
	tst_res(TINFO, "Testing variant: %s", tv->desc);
	start.type = end.type = sleep_abs.type = tv->ts_type;
	unshare(CLONE_NEWTIME);
	fprintf("/proc/self/timens_offsets", "%d %d 0", CLOCK_MONOTONIC, OFFSET_S);
	do_clock_gettime(tv, &start);
	sleep_abs = tst_ts_add_us(start, 1000000 * OFFSET_S + SLEEP_US);
	if (!fork()) {
tv->clock_nanosleep(CLOCK_MONOTONIC, TIMER_ABSTIME, tst_ts_get(&sleep_abs), NULL);
		 * The return value and error number are differently set for
		 * libc syscall as compared to kernel syscall.
		 */
		if ((tv->clock_nanosleep == libc_clock_nanosleep) && TST_RET) {
			TST_ERR = TST_RET;
			TST_RET = -1;
		}
		if (TST_RET == -1) {
			tst_res(TFAIL | TTERRNO, "clock_nanosleep(2) failed for clock %s",
				tst_clock_name(CLOCK_MONOTONIC));
		}
		exit(0);
	}
	wait(NULL);
	do_clock_gettime(tv, &end);
	long long diff = tst_ts_diff_us(end, start);
	if (diff > 5 * SLEEP_US) {
		tst_res(TFAIL, "clock_nanosleep() slept too long %lli", diff);
		return;
	}
	if (diff < SLEEP_US) {
		tst_res(TFAIL, "clock_nanosleep() slept too short %lli", diff);
		return;
	}
	tst_res(TPASS, "clock_nanosleep() slept correctly %lli", diff);
}

void main(){
	setup();
	cleanup();
}
