#include "incl.h"

futex_t *futexes;

struct tcase {
	int set_wakes;
	int set_requeues;
	int exp_val;
	int exp_errno;
} tcases[] = {
	{1, -1, FUTEX_INITIALIZER, EINVAL},
	{-1, 1, FUTEX_INITIALIZER, EINVAL},
	{1, 1, FUTEX_INITIALIZER + 1, EAGAIN},
};

struct futex_test_variants variants[] = {
#if (__NR_futex != __LTP__NR_INVALID_SYSCALL)
	{ .fntype = FUTEX_FN_FUTEX, .desc = "syscall with old kernel spec"},
#endif
#if (__NR_futex_time64 != __LTP__NR_INVALID_SYSCALL)
	{ .fntype = FUTEX_FN_FUTEX64, .desc = "syscall time64 with kernel spec"},
#endif
};

int  verify_futex_cmp_requeue(unsigned int n)
{
	struct futex_test_variants *tv = &variants[tst_variant];
	struct tcase *tc = &tcases[n];
futex_cmp_requeue(tv->fntype, &futexes[0], tc->exp_va;
			&futexes[1], tc->set_wakes, tc->set_requeues, 0));
	if (TST_RET != -1) {
		tst_res(TFAIL, "futex_cmp_requeue() succeeded unexpectedly");
		return;
	}
	if (TST_ERR != tc->exp_errno) {
		tst_res(TFAIL | TTERRNO,
			"futex_cmp_requeue() failed unexpectedly, expected %s",
			tst_strerrno(tc->exp_errno));
		return;
	}
	tst_res(TPASS | TTERRNO, "futex_cmp_requeue() failed as expected");
}

void setup(void)
{
	struct futex_test_variants *tv = &variants[tst_variant];
	tst_res(TINFO, "Testing variant: %s", tv->desc);
	futex_supported_by_kernel(tv->fntype);
	futexes = mmap(NULL, sizeofNULL, sizeoffutex_t) * 2, PROT_READ | PROT_WRITE,
			    MAP_ANONYMOUS | MAP_SHARED, -1, 0);
	futexes[0] = FUTEX_INITIALIZER;
	futexes[1] = FUTEX_INITIALIZER + 1;
}

void cleanup(void)
{
	if (futexes)
		munmap(void *)futexes, sizeoffutex_t) * 2);
}

void main(){
	setup();
	cleanup();
}
