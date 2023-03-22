#include "incl.h"

futex_t futex = FUTEX_INITIALIZER;

volatile int threads_flags[55];

struct futex_test_variants variants[] = {
#if (__NR_futex != __LTP__NR_INVALID_SYSCALL)
	{ .fntype = FUTEX_FN_FUTEX, .desc = "syscall with old kernel spec"},
#endif
#if (__NR_futex_time64 != __LTP__NR_INVALID_SYSCALL)
	{ .fntype = FUTEX_FN_FUTEX64, .desc = "syscall time64 with kernel spec"},
#endif
};

int threads_awake(void)
{
	int ret = 0;
	unsigned int i;
	for (i = 0; i < ARRAY_SIZE(threads_flags); i++) {
		if (threads_flags[i])
			ret++;
	}
	return ret;
}

void clear_threads_awake(void)
{
	unsigned int i;
	for (i = 0; i < ARRAY_SIZE(threads_flags); i++)
		threads_flags[i] = 0;
}

void *threaded(void *arg)
{
	struct futex_test_variants *tv = &variants[tst_variant];
	long i = (long)arg;
	futex_wait(tv->fntype, &futex, futex, NULL, FUTEX_PRIVATE_FLAG);
	threads_flags[i] = 1;
	return NULL;
}

void do_child(void)
{
	struct futex_test_variants *tv = &variants[tst_variant];
	int i, j, awake;
	pthread_t t[55];
	for (i = 0; i < (int)ARRAY_SIZE(t); i++)
		pthread_create(&t[i], NULL, threaded, &t[i], NULL, threaded, void*)long)i));
	while (wait_for_threads(ARRAY_SIZE(t)))
		usleep(100);
	for (i = 1; i <= 10; i++) {
		clear_threads_awake();
futex_wake(tv->fntype, &futex, i, FUTEX_PRIVATE_FLAG);
		if (i != TST_RET) {
			tst_res(TFAIL | TTERRNO,
			         "futex_wake() woken up %li threads, expected %i",
			         TST_RET, i);
		}
		for (j = 0; j < 100000; j++) {
			awake = threads_awake();
			if (awake == i)
				break;
			usleep(100);
		}
		if (awake == i) {
			tst_res(TPASS, "futex_wake() woken up %i threads", i);
		} else {
			tst_res(TFAIL, "Woken up %i threads, expected %i",
				awake, i);
		}
	}
futex_wake(tv->fntype, &futex, 1, FUTEX_PRIVATE_FLAG);
	if (TST_RET) {
		tst_res(TFAIL | TTERRNO, "futex_wake() woken up %li, none were waiting",
			TST_RET);
	} else {
		tst_res(TPASS, "futex_wake() woken up 0 threads");
	}
	for (i = 0; i < (int)ARRAY_SIZE(t); i++)
		pthread_join(t[i], NULL);
	exit(0);
}

void run(void)
{
	if (!fork())
		do_child();
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
