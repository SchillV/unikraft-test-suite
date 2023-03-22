#include "incl.h"
struct test_case {
	clockid_t clktype;
	int allow_inval;
};

struct test_case tc[] = {
	{
	 .clktype = CLOCK_REALTIME,
	 },
	{
	 .clktype = CLOCK_MONOTONIC,
	 },
	{
	 .clktype = CLOCK_PROCESS_CPUTIME_ID,
	 },
	{
	 .clktype = CLOCK_THREAD_CPUTIME_ID,
	 },
	{
	 .clktype = CLOCK_REALTIME_COARSE,
	 .allow_inval = 1,
	 },
	{
	 .clktype = CLOCK_MONOTONIC_COARSE,
	 .allow_inval = 1,
	 },
	{
	 .clktype = CLOCK_MONOTONIC_RAW,
	 .allow_inval = 1,
	 },
	{
	 .clktype = CLOCK_BOOTTIME,
	 .allow_inval = 1,
	 },
};

struct tst_ts spec;

struct time64_variants variants[] = {
	{ .clock_gettime = libc_clock_gettime, .ts_type = TST_LIBC_TIMESPEC, .desc = "vDSO or syscall with libc spec"},
#if (__NR_clock_gettime != __LTP__NR_INVALID_SYSCALL)
	{ .clock_gettime = sys_clock_gettime, .ts_type = TST_KERN_OLD_TIMESPEC, .desc = "syscall with old kernel spec"},
#endif
#if (__NR_clock_gettime64 != __LTP__NR_INVALID_SYSCALL)
	{ .clock_gettime = sys_clock_gettime64, .ts_type = TST_KERN_TIMESPEC, .desc = "syscall time64 with kernel spec"},
#endif
};

void setup(void)
{
	long unsigned utime;
	tst_res(TINFO, "Testing variant: %s", variants[tst_variant].desc);
	do {
		file_scanf("/proc/self/stat",
			"%*d %*s %*c %*d %*d %*d %*d %*d %*u %*u %*u %*u %*u %lu",
			&utime);
	} while (utime == 0);
}

int  verify_clock_gettime(unsigned int i)
{
	struct time64_variants *tv = &variants[tst_variant];
	int ret;
	memset(&spec, 0, sizeof(spec));
	spec.type = tv->ts_type;
tv->clock_gettime(tc[i].clktype, tst_ts_get(&spec));
	if (TST_RET == -1) {
		if (tc[i].allow_inval && TST_ERR == EINVAL) {
			tst_res(TPASS, "clock_gettime(2): unsupported clock %s failed as expected",
				tst_clock_name(tc[i].clktype));
		} else {
			tst_res(TFAIL | TTERRNO, "clock_gettime(2): clock %s failed unexpectedly",
				tst_clock_name(tc[i].clktype));
		}
	} else {
		ret = tst_ts_valid(&spec);
		if (!ret) {
			tst_res(TPASS, "clock_gettime(2): clock %s passed",
				tst_clock_name(tc[i].clktype));
		} else if (ret == -1) {
			tst_res(TFAIL, "clock_gettime(2): clock %s passed, unchanged timespec",
				tst_clock_name(tc[i].clktype));
		} else if (ret == -2) {
			tst_res(TFAIL, "clock_gettime(2): clock %s passed, Corrupted timespec",
				tst_clock_name(tc[i].clktype));
		}
	}
}

void main(){
	setup();
	cleanup();
}
