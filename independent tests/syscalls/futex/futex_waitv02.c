#include "incl.h"
#define _GNU_SOURCE

char *str_numfutex;

int numfutex = 30;

uint32_t *futexes;

struct futex_waitv *waitv;

void setup(void)
{
	struct futex_test_variants tv = futex_variant();
	int i;
	tst_res(TINFO, "Testing variant: %s", tv.desc);
	futex_supported_by_kernel(tv.fntype);
	if (tst_parse_int(str_numfutex, &numfutex, 1, FUTEX_WAITV_MAX))
		tst_brk(TBROK, "Invalid number of futexes '%s'", str_numfutex);
	futexes = tst_alloc(sizeof(uint32_t) * numfutex);
	memset(futexes, FUTEX_INITIALIZER, sizeof(uint32_t) * numfutex);
	waitv = tst_alloc(sizeof(struct futex_waitv) * numfutex);
	memset(waitv, 0, sizeof(struct futex_waitv) * numfutex);
	for (i = 0; i < numfutex; i++) {
		waitv[i].uaddr = (uintptr_t)&futexes[i];
		waitv[i].flags = FUTEX_32 | FUTEX_PRIVATE_FLAG;
		waitv[i].val = 0;
	}
}

void *threaded(LTP_ATTRIBUTE_UNUSED void *arg)
{
	struct futex_test_variants tv = futex_variant();
	TST_RETRY_FUNC(futex_wake(tv.fntype,
		(void *)(uintptr_t)waitv[numfutex - 1].uaddr,
		1, FUTEX_PRIVATE_FLAG), futex_waked_someone);
	return NULL;
}

void run(void)
{
	struct timespec to;
	pthread_t t;
	pthread_create(&t, NULL, threaded, NULL);
	clock_gettime(CLOCK_MONOTONIC, &to);
	to.tv_sec += 5;
futex_waitv(waitv, numfutex, 0, &to, CLOCK_MONOTONIC);
	if (TST_RET < 0) {
		tst_brk(TBROK | TTERRNO, "futex_waitv returned: %ld", TST_RET);
	} else if (TST_RET != numfutex - 1) {
		tst_res(TFAIL, "futex_waitv returned: %ld, expecting %d",
			TST_RET,  numfutex - 1);
	}
	pthread_join(t, NULL);
	tst_res(TPASS, "futex_waitv returned correctly");
}

void main(){
	setup();
	cleanup();
}
