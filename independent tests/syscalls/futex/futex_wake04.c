#include "incl.h"

futex_t *futex1, *futex2;

struct tst_ts to;

struct futex_test_variants variants[] = {
#if (__NR_futex != __LTP__NR_INVALID_SYSCALL)
	{ .fntype = FUTEX_FN_FUTEX, .tstype = TST_KERN_OLD_TIMESPEC, .desc = "syscall with old kernel spec"},
#endif
#if (__NR_futex_time64 != __LTP__NR_INVALID_SYSCALL)
	{ .fntype = FUTEX_FN_FUTEX64, .tstype = TST_KERN_TIMESPEC, .desc = "syscall time64 with kernel spec"},
#endif
};

void setup(void)
{
	struct futex_test_variants *tv = &variants[tst_variant];
	tst_res(TINFO, "Testing variant: %s", tv->desc);
	futex_supported_by_kernel(tv->fntype);
	to = tst_ts_from_ns(tv->tstype, 30 * NSEC_PER_SEC);
}

void *wait_thread1(void *arg LTP_ATTRIBUTE_UNUSED)
{
	struct futex_test_variants *tv = &variants[tst_variant];
	futex_wait(tv->fntype, futex1, *futex1, &to, 0);
	return NULL;
}

void *wait_thread2(void *arg LTP_ATTRIBUTE_UNUSED)
{
	struct futex_test_variants *tv = &variants[tst_variant];
	int res;
	errno = 0;
	res = futex_wait(tv->fntype, futex2, *futex2, &to, 0);
	if (!res)
		tst_res(TPASS, "Hi hydra, thread2 awake!");
	else
		tst_res(TFAIL | TERRNO, "Bug: wait_thread2 did not wake after 30 secs.");
	return NULL;
}

void wakeup_thread2(void)
{
	struct futex_test_variants *tv = &variants[tst_variant];
	void *addr;
	int hpsz, pgsz;
	pthread_t th1, th2;
	hpsz = tst_get_hugepage_size();
	tst_res(TINFO, "Hugepagesize %i", hpsz);
	addr = mmap(NULL, hpsz, PROT_WRITE | PROT_READ,
	            MAP_SHARED | MAP_ANONYMOUS | MAP_HUGETLB, -1, 0);
	if (addr == MAP_FAILED) {
		if (errno == ENOMEM)
			tst_brk(TCONF, "Cannot allocate hugepage, memory too fragmented?");
		tst_brk(TBROK | TERRNO, "Cannot allocate hugepage");
	}
	pgsz = getpagesize();
	futex1 = addr;
	futex2 = (futex_t *)((char *)addr + pgsz);
	pthread_create(&th1, NULL, wait_thread1, NULL);
	pthread_create(&th2, NULL, wait_thread2, NULL);
	while (wait_for_threads(2))
		usleep(1000);
	futex_wake(tv->fntype, futex2, 1, 0);
	pthread_join(th2, NULL);
	futex_wake(tv->fntype, futex1, 1, 0);
	pthread_join(th1, NULL);
	munmap(addr, hpsz);
}

void main(){
	setup();
	cleanup();
}
