#include "incl.h"

void *bad_addr;
struct test_case {
	clockid_t clktype;
	int exp_err;
	int allow_inval;
};

struct test_case tc[] = {
	{
	 .clktype = MAX_CLOCKS,
	 .exp_err = EINVAL,
	 },
	{
	 .clktype = MAX_CLOCKS + 1,
	 .exp_err = EINVAL,
	 },
	 * Different POSIX clocks have different (*clock_get)() handlers.
	 * It justifies testing EFAULT for all.
	 */
	{
	 .clktype = CLOCK_REALTIME,
	 .exp_err = EFAULT,
	 },
	{
	 .clktype = CLOCK_MONOTONIC,
	 .exp_err = EFAULT,
	 },
	{
	 .clktype = CLOCK_PROCESS_CPUTIME_ID,
	 .exp_err = EFAULT,
	 },
	{
	 .clktype = CLOCK_THREAD_CPUTIME_ID,
	 .exp_err = EFAULT,
	 },
	{
	 .clktype = CLOCK_REALTIME_COARSE,
	 .exp_err = EFAULT,
	 .allow_inval = 1,
	 },
	{
	 .clktype = CLOCK_MONOTONIC_COARSE,
	 .exp_err = EFAULT,
	 .allow_inval = 1,
	 },
	{
	 .clktype = CLOCK_MONOTONIC_RAW,
	 .exp_err = EFAULT,
	 .allow_inval = 1,
	 },
	{
	 .clktype = CLOCK_BOOTTIME,
	 .exp_err = EFAULT,
	 .allow_inval = 1,
	 },
};

struct tst_ts spec;

struct time64_variants variants[] = {
#if (__NR_clock_gettime != __LTP__NR_INVALID_SYSCALL)
	{ .clock_gettime = sys_clock_gettime, .ts_type = TST_KERN_OLD_TIMESPEC, .desc = "syscall with old kernel spec"},
#endif
#if (__NR_clock_gettime64 != __LTP__NR_INVALID_SYSCALL)
	{ .clock_gettime = sys_clock_gettime64, .ts_type = TST_KERN_TIMESPEC, .desc = "syscall time64 with kernel spec"},
#endif
};

void setup(void)
{
	tst_res(TINFO, "Testing variant: %d: %s", tst_variant, variants[tst_variant].desc);
	bad_addr = tst_get_bad_addr(NULL);
}

int  verify_clock_gettime(unsigned int i)
{
	struct time64_variants *tv = &variants[tst_variant];
	void *ts;
	if (tc[i].exp_err == EFAULT) {
		ts = bad_addr;
	} else {
		spec.type = tv->ts_type;
		ts = tst_ts_get(&spec);
	}
tv->clock_gettime(tc[i].clktype, ts);
	if (TST_RET != -1) {
		tst_res(TFAIL, "clock_gettime(2): clock %s passed unexpectedly",
			tst_clock_name(tc[i].clktype));
		return;
	}
	if ((tc[i].exp_err == TST_ERR) ||
	    (tc[i].allow_inval && TST_ERR == EINVAL)) {
		tst_res(TPASS | TTERRNO, "clock_gettime(2): clock %s failed as expected",
			tst_clock_name(tc[i].clktype));
	} else {
		tst_res(TFAIL | TTERRNO, "clock_gettime(2): clock %s failed unexpectedly",
			tst_clock_name(tc[i].clktype));
	}
}

void main(){
	setup();
	cleanup();
}
