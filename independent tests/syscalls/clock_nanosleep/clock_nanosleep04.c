#include "incl.h"

clockid_t tcase[] = {
	CLOCK_MONOTONIC,
	CLOCK_REALTIME,
};

struct time64_variants variants[] = {
	{ .clock_gettime = libc_clock_gettime, .clock_nanosleep = libc_clock_nanosleep, .ts_type = TST_LIBC_TIMESPEC, .desc = "vDSO or syscall with libc spec"},
#if (__NR_clock_nanosleep != __LTP__NR_INVALID_SYSCALL)
	{ .clock_gettime = sys_clock_gettime, .clock_nanosleep = sys_clock_nanosleep, .ts_type = TST_KERN_OLD_TIMESPEC, .desc = "syscall with old kernel spec"},
#endif
#if (__NR_clock_nanosleep_time64 != __LTP__NR_INVALID_SYSCALL)
	{ .clock_gettime = sys_clock_gettime64, .clock_nanosleep = sys_clock_nanosleep64, .ts_type = TST_KERN_TIMESPEC, .desc = "syscall time64 with kernel spec"},
#endif
};
void setup(void)
{
	tst_res(TINFO, "Testing variant: %s", variants[tst_variant].desc);
}

void do_test(unsigned int i)
{
	struct time64_variants *tv = &variants[tst_variant];
	struct tst_ts ts = {.type = tv->ts_type};
tv->clock_gettime(tcase[i], tst_ts_get(&ts));
	if (TST_RET == -1) {
		tst_res(TFAIL | TTERRNO, "clock_gettime(2) failed for clock %s",
			tst_clock_name(tcase[i]));
		return;
	}
	ts = tst_ts_add_us(ts, 10000);
tv->clock_nanosleep(tcase[i], TIMER_ABSTIME, tst_ts_get(&ts), NULL);
	if (TST_RET) {
		tst_res(TFAIL | TTERRNO, "clock_nanosleep(2) failed for clock %s",
			tst_clock_name(tcase[i]));
	}
	tst_res(TPASS, "clock_nanosleep(2) passed for clock %s",
		tst_clock_name(tcase[i]));
}

void main(){
	setup();
	cleanup();
}
