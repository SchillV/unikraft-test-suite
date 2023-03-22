#include "incl.h"
#define _GNU_SOURCE

int bad_clockfd = -1;

int clockfd;

int fd;

void *bad_addr;

struct test_case_t {
	int *fd;
	int flags;
	struct tst_its *old_value;
	int exp_errno;
} test_cases[] = {
	{&bad_clockfd, 0, NULL, EBADF},
	{&clockfd, 0, NULL, EFAULT},
	{&fd, 0, NULL, EINVAL},
	{&clockfd, -1, NULL, EINVAL},
};

struct tst_its new_value;

struct time64_variants variants[] = {
#if (__NR_timerfd_settime != __LTP__NR_INVALID_SYSCALL)
	{ .tfd_settime = sys_timerfd_settime, .ts_type = TST_KERN_OLD_TIMESPEC, .desc = "syscall with old kernel spec"},
#endif
#if (__NR_timerfd_settime64 != __LTP__NR_INVALID_SYSCALL)
	{ .tfd_settime = sys_timerfd_settime64, .ts_type = TST_KERN_TIMESPEC, .desc = "syscall time64 with kernel spec"},
#endif
};

void setup(void)
{
	struct time64_variants *tv = &variants[tst_variant];
	tst_res(TINFO, "Testing variant: %s", tv->desc);
	bad_addr = tst_get_bad_addr(NULL);
	new_value.type = tv->ts_type;
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
		its = tst_its_get(test->old_value);
tv->tfd_settime(*test->fd, test->flags, tst_its_get(&new_value;
			     its));
	if (TST_RET != -1) {
		tst_res(TFAIL, "timerfd_settime() succeeded unexpectedly");
		return;
	}
	if (TST_ERR == test->exp_errno) {
		tst_res(TPASS | TTERRNO,
			"timerfd_settime() failed as expected");
	} else {
		tst_res(TFAIL | TTERRNO,
			"timerfd_settime() failed unexpectedly; expected: "
			"%d - %s", test->exp_errno, strerror(test->exp_errno));
	}
}

void main(){
	setup();
	cleanup();
}
