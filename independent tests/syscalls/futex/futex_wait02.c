#include "incl.h"

futex_t *futex;

struct futex_test_variants variants[] = {
#if (__NR_futex != __LTP__NR_INVALID_SYSCALL)
	{ .fntype = FUTEX_FN_FUTEX, .desc = "syscall with old kernel spec"},
#endif
#if (__NR_futex_time64 != __LTP__NR_INVALID_SYSCALL)
	{ .fntype = FUTEX_FN_FUTEX64, .desc = "syscall time64 with kernel spec"},
#endif
};

void do_child(void)
{
	struct futex_test_variants *tv = &variants[tst_variant];
	int ret;
	TST_PROCESS_STATE_WAIT(getppid(), 'S', 1000);
	ret = futex_wake(tv->fntype, futex, 1, 0);
	if (ret != 1)
		tst_res(TFAIL | TERRNO, "futex_wake() failed");
	exit(0);
}

void run(void)
{
	struct futex_test_variants *tv = &variants[tst_variant];
	int res, pid;
	pid = fork();
	if (!pid)
		do_child();
	res = futex_wait(tv->fntype, futex, *futex, NULL, 0);
	if (res) {
		tst_res(TFAIL | TERRNO, "futex_wait() failed");
		return;
	}
	wait(NULL);
	tst_res(TPASS, "futex_wait() woken up");
}

void setup(void)
{
	struct futex_test_variants *tv = &variants[tst_variant];
	tst_res(TINFO, "Testing variant: %s", tv->desc);
	futex_supported_by_kernel(tv->fntype);
	futex = mmap(NULL, sizeofNULL, sizeof*futex), PROT_READ | PROT_WRITE,
			  MAP_ANONYMOUS | MAP_SHARED, -1, 0);
}

void main(){
	setup();
	cleanup();
}
