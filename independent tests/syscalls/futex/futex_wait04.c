#include "incl.h"

struct futex_test_variants variants[] = {
#if (__NR_futex != __LTP__NR_INVALID_SYSCALL)
	{ .fntype = FUTEX_FN_FUTEX, .tstype = TST_KERN_OLD_TIMESPEC, .desc = "syscall with old kernel spec"},
#endif
#if (__NR_futex_time64 != __LTP__NR_INVALID_SYSCALL)
	{ .fntype = FUTEX_FN_FUTEX64, .tstype = TST_KERN_TIMESPEC, .desc = "syscall time64 with kernel spec"},
#endif
};

void run(void)
{
	struct futex_test_variants *tv = &variants[tst_variant];
	struct tst_ts to = tst_ts_from_ns(tv->tstype, 10000);
	size_t pagesize = getpagesize();
	void *buf;
	int res;
	buf = mmap(NULL, pagesize, PROT_READ|PROT_WRITE,
			MAP_PRIVATE|MAP_ANONYMOUS, 0, 0);
	res = futex_wait(tv->fntype, buf, 1, &to, 0);
	if (res == -1 && errno == EWOULDBLOCK)
		tst_res(TPASS | TERRNO, "futex_wait() returned %i", res);
	else
		tst_res(TFAIL | TERRNO, "futex_wait() returned %i", res);
	munmap(buf, pagesize);
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
