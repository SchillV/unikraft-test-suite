#include "incl.h"

futex_t futex = FUTEX_INITIALIZER;

struct futex_test_variants variants[] = {
#if (__NR_futex != __LTP__NR_INVALID_SYSCALL)
	{ .fntype = FUTEX_FN_FUTEX, .desc = "syscall with old kernel spec"},
#endif
#if (__NR_futex_time64 != __LTP__NR_INVALID_SYSCALL)
	{ .fntype = FUTEX_FN_FUTEX64, .desc = "syscall time64 with kernel spec"},
#endif
};

void *threaded(void *arg)
{
	struct futex_test_variants *tv = &variants[tst_variant];
	long ret, pid = (long)arg;
	TST_PROCESS_STATE_WAIT(pid, 'S', 0);
	ret = futex_wake(tv->fntype, &futex, 1, FUTEX_PRIVATE_FLAG);
	if (ret != 1)
		tst_res(TFAIL, "futex_wake() returned %li", ret);
	return (void*)ret;
}

void run(void)
{
	struct futex_test_variants *tv = &variants[tst_variant];
	long res, pid = getpid();
	pthread_t t;
	pthread_create(&t, NULL, threaded, &t, NULL, threaded, void*)pid);
	res = futex_wait(tv->fntype, &futex, futex, NULL, FUTEX_PRIVATE_FLAG);
	if (res) {
		tst_res(TFAIL | TERRNO, "futex_wait() failed");
		pthread_join(t, NULL);
		return;
	}
	pthread_join(t, NULL);
	tst_res(TPASS, "futex_wait() woken up");
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
