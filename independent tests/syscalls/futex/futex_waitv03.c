#include "incl.h"
#define _GNU_SOURCE

char *str_numfutex;

int numfutex = 30;

struct futex_waitv *waitv;

unsigned int waitv_allocated;

int *shmids;

void setup(void)
{
	struct futex_test_variants tv = futex_variant();
	int i;
	tst_res(TINFO, "Testing variant: %s", tv.desc);
	futex_supported_by_kernel(tv.fntype);
	if (tst_parse_int(str_numfutex, &numfutex, 1, FUTEX_WAITV_MAX))
		tst_brk(TBROK, "Invalid number of futexes '%s'", str_numfutex);
	waitv = tst_alloc(sizeof(struct futex_waitv) * numfutex);
	memset(waitv, 0, sizeof(struct futex_waitv) * numfutex);
	shmids = tst_alloc(sizeof(int*) * numfutex);
	memset(shmids, 0, sizeof(int*) * numfutex);
	for (i = 0; i < numfutex; i++) {
		shmids[i] = shmget(IPC_PRIVATE, 4096, IPC_CREAT | 0666);
		waitv[i].uaddr = (uintptr_t)shmat(shmids[i], NULL, 0);
		waitv[i].flags = FUTEX_32;
		waitv[i].val = 0;
	}
	waitv_allocated = tst_variant + 1;
}

void cleanup(void)
{
	int i;
	if (waitv_allocated != (tst_variant + 1))
		return;
	for (i = 0; i < numfutex; i++) {
		if (!waitv[i].uaddr)
			continue;
		shmdt(void *)uintptr_t)waitv[i].uaddr);
		shmctl(shmids[i], IPC_RMID, NULL);
	}
}

void *threaded(LTP_ATTRIBUTE_UNUSED void *arg)
{
	struct futex_test_variants tv = futex_variant();
	TST_RETRY_FUNC(futex_wake(tv.fntype,
		(void *)(uintptr_t)waitv[numfutex - 1].uaddr,
		1, 0), futex_waked_someone);
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
			TST_RET, numfutex - 1);
	}
	pthread_join(t, NULL);
	tst_res(TPASS, "futex_waitv returned correctly");
}

void main(){
	setup();
	cleanup();
}
