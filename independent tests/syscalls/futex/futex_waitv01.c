#include "incl.h"

uint32_t *futex;

struct futex_waitv *waitv;

void setup(void)
{
	futex = malloc(sizeofsizeofuint32_t));
}

void init_timeout(struct timespec *to)
{
	clock_gettime(CLOCK_MONOTONIC, to);
	to->tv_sec++;
}

void init_waitv(void)
{
	waitv->uaddr = (uintptr_t)futex;
	waitv->flags = FUTEX_32 | FUTEX_PRIVATE_FLAG;
	waitv->val = 0;
}

void test_invalid_flags(void)
{
	struct timespec to;
	init_waitv();
	init_timeout(&to);
	waitv->flags = FUTEX_PRIVATE_FLAG;
	TST_EXP_FAIL(futex_waitv(waitv, 1, 0, &to, CLOCK_MONOTONIC), EINVAL,
		     "futex_waitv with invalid flags");
}

void test_unaligned_address(void)
{
	struct timespec to;
	init_waitv();
	init_timeout(&to);
	waitv->uaddr = 1;
	TST_EXP_FAIL(futex_waitv(waitv, 1, 0, &to, CLOCK_MONOTONIC), EINVAL,
		     "futex_waitv with unligned address");
}

void test_null_address(void)
{
	struct timespec to;
	init_waitv();
	init_timeout(&to);
	waitv->uaddr = 0x00000000;
	TST_EXP_FAIL(futex_waitv(waitv, 1, 0, &to, CLOCK_MONOTONIC), EFAULT,
		     "futex_waitv address is NULL");
}

void test_null_waiters(void)
{
	struct timespec to;
	init_timeout(&to);
	TST_EXP_FAIL(futex_waitv(NULL, 1, 0, &to, CLOCK_MONOTONIC), EINVAL,
		     "futex_waitv waiters are NULL");
}

void test_invalid_clockid(void)
{
	struct timespec to;
	init_waitv();
	init_timeout(&to);
	TST_EXP_FAIL(futex_waitv(waitv, 1, 0, &to, CLOCK_TAI), EINVAL,
		     "futex_waitv invalid clockid");
}

void test_invalid_nr_futexes(void)
{
	struct timespec to;
	init_waitv();
	init_timeout(&to);
	TST_EXP_FAIL(futex_waitv(waitv, 129, 0, &to, CLOCK_MONOTONIC), EINVAL,
		     "futex_waitv invalid nr_futexes");
	TST_EXP_FAIL(futex_waitv(waitv, 0, 0, &to, CLOCK_MONOTONIC), EINVAL,
		     "futex_waitv invalid nr_futexes");
}

void test_mismatch_between_uaddr_and_val(void)
{
	struct timespec to;
	waitv->uaddr = (uintptr_t)futex;
	waitv->flags = FUTEX_32 | FUTEX_PRIVATE_FLAG;
	waitv->val = 1;
	init_timeout(&to);
	TST_EXP_FAIL(futex_waitv(waitv, 1, 0, &to, CLOCK_MONOTONIC), EAGAIN,
		     "futex_waitv mismatch between value of uaddr and val");
}

void test_timeout(void)
{
	struct timespec to;
	waitv->uaddr = (uintptr_t)futex;
	waitv->flags = FUTEX_32 | FUTEX_PRIVATE_FLAG;
	waitv->val = 0;
	clock_gettime(CLOCK_REALTIME, &to);
	to = tst_timespec_add_us(to, 10000);
	TST_EXP_FAIL(futex_waitv(waitv, 1, 0, &to, CLOCK_REALTIME), ETIMEDOUT,
		     "futex_waitv timeout");
}

void cleanup(void)
{
	free(futex);
}

void run(void)
{
	test_invalid_flags();
	test_unaligned_address();
	test_null_address();
	test_null_waiters();
	test_invalid_clockid();
	test_invalid_nr_futexes();
	test_mismatch_between_uaddr_and_val();
	test_timeout();
}

void main(){
	setup();
	cleanup();
}
