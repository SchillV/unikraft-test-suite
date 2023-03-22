#include "incl.h"
#define _GNU_SOURCE
char *TCID = "timerfd_gettime01";

int bad_clockfd = -1;

int clockfd;

int fd;

void *bad_addr;

struct test_case_t {
	int *fd;
	struct tst_its *curr_value;
	int exp_errno;
} test_cases[] = {
	{&bad_clockfd, NULL, EBADF},
	{&clockfd, NULL, EFAULT},
	{&fd, NULL, EINVAL},
};

struct time64_variants variants[] = {
#if (__NR_timerfd_gettime != __LTP__NR_INVALID_SYSCALL)
	{ .tfd_gettime = sys_timerfd_gettime, .desc = "syscall with old kernel spec"},
#endif
#if (__NR_timerfd_gettime64 != __LTP__NR_INVALID_SYSCALL)
	{ .tfd_gettime = sys_timerfd_gettime64, .desc = "syscall time64 with kernel spec"},
#endif
};

void setup(void)
{
	tst_res(TINFO, "Testing variant: %s", variants[tst_variant].desc);
	bad_addr = tst_get_bad_addr(NULL);
	clockfd = timerfd_create(CLOCK_REALTIME, 0);
	if (clockfd == -1) {
		tst_brk(TFAIL | TERRNO, "timerfd_create() fail");
		return;
	}
	fd = open("test_file", O_RDWR | O_CREAT, 0644);
}

void cleanup(void)
{
	if (clockfd > 0)
		close(clockfd);
	if (fd > 0)
		close(fd);
}

void run(unsigned int n)
{
	struct time64_variants *tv = &variants[tst_variant];
	struct test_case_t *test = &test_cases[n];
	void *its;
	if (test->exp_errno == EFAULT)
		its = bad_addr;
	else
		its = tst_its_get(test->curr_value);
tv->tfd_gettime(*test->fd, its);
	if (TST_RET != -1) {
		tst_res(TFAIL, "timerfd_gettime() succeeded unexpectedly");
		return;
	}
	if (TST_ERR == test->exp_errno) {
		tst_res(TPASS | TTERRNO,
			"timerfd_gettime() failed as expected");
	} else {
		tst_res(TFAIL | TTERRNO,
			"timerfd_gettime() failed unexpectedly; expected: "
			"%d - %s", test->exp_errno, strerror(test->exp_errno));
	}
}

void main(){
	setup();
	cleanup();
}
