#include "incl.h"
struct testcase {
	futex_t *f_addr;
	futex_t f_val;
	int opflags;
	int exp_errno;
};

futex_t futex = FUTEX_INITIALIZER;

struct testcase testcases[] = {
	{&futex, FUTEX_INITIALIZER, 0, ETIMEDOUT},
	{&futex, FUTEX_INITIALIZER+1, 0, EWOULDBLOCK},
	{&futex, FUTEX_INITIALIZER, FUTEX_PRIVATE_FLAG, ETIMEDOUT},
	{&futex, FUTEX_INITIALIZER+1, FUTEX_PRIVATE_FLAG, EWOULDBLOCK},
};

struct futex_test_variants variants[] = {
#if (__NR_futex != __LTP__NR_INVALID_SYSCALL)
	{ .fntype = FUTEX_FN_FUTEX, .tstype = TST_KERN_OLD_TIMESPEC, .desc = "syscall with old kernel spec"},
#endif
#if (__NR_futex_time64 != __LTP__NR_INVALID_SYSCALL)
	{ .fntype = FUTEX_FN_FUTEX64, .tstype = TST_KERN_TIMESPEC, .desc = "syscall time64 with kernel spec"},
#endif
};

void run(unsigned int n)
{
	struct futex_test_variants *tv = &variants[tst_variant];
	struct testcase *tc = &testcases[n];
	struct tst_ts to = tst_ts_from_ns(tv->tstype, 10000);
	int res;
	res = futex_wait(tv->fntype, tc->f_addr, tc->f_val, &to, tc->opflags);
	if (res != -1) {
		tst_res(TFAIL, "futex_wait() succeeded unexpectedly");
		return;
	}
	if (errno != tc->exp_errno) {
		tst_res(TFAIL | TERRNO, "futex_wait() failed with incorrect error, expected errno=%s",
		         tst_strerrno(tc->exp_errno));
		return;
	}
	tst_res(TPASS | TERRNO, "futex_wait() passed");
}

void setup(void)
{
	struct futex_test_variants *tv = &variants[tst_variant];
	tst_res(TINFO, "Testing variant: %s", tv->desc);
	futex_supported_by_kernel(tv->fntype);
}

void main(){
	setup();
	cleanup();
}
