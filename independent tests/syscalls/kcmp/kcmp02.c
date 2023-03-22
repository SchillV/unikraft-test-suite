#include "incl.h"
#define _GNU_SOURCE
#define TEST_FILE "test_file"
#define TEST_FILE2 "test_file2"

int fd1;

int fd2;

int fd_fake;

int pid1;

int pid_unused;

int fd_fake = -1;

struct test_case {
	int *pid1;
	int *pid2;
	int type;
	int *fd1;
	int *fd2;
	int exp_errno;
} test_cases[] = {
	{&pid1, &pid_unused, KCMP_FILE, &fd1, &fd2, ESRCH},
	{&pid1, &pid1, KCMP_TYPES + 1, &fd1, &fd2, EINVAL},
	{&pid1, &pid1, -1, &fd1, &fd2, EINVAL},
	{&pid1, &pid1, INT_MIN, &fd1, &fd2, EINVAL},
	{&pid1, &pid1, INT_MAX, &fd1, &fd2, EINVAL},
	{&pid1, &pid1, KCMP_FILE, &fd1, &fd_fake, EBADF}
};

void setup(void)
{
	pid1 = getpid();
	pid_unused = tst_get_unused_pid();
	fd1 = open(TEST_FILE, O_CREAT | O_RDWR | O_TRUNC, 0644);
	fd2 = open(TEST_FILE2, O_CREAT | O_RDWR | O_TRUNC, 0644);
}

void cleanup(void)
{
	if (fd1 > 0)
		close(fd1);
	if (fd2 > 0)
		close(fd2);
}

int  verify_kcmp(unsigned int n)
{
	struct test_case *test = &test_cases[n];
kcmp(*(test->pid1), *(test->pid2), test->typ;
		  *(test->fd1), *(test->fd2)));
	if (TST_RET != -1) {
		tst_res(TFAIL, "kcmp() succeeded unexpectedly");
		return;
	}
	if (test->exp_errno == TST_ERR) {
		tst_res(TPASS | TTERRNO, "kcmp() returned the expected value");
		return;
	}
	tst_res(TFAIL | TTERRNO,
		"kcmp() got unexpected return value: expected: %d - %s",
			test->exp_errno, tst_strerrno(test->exp_errno));
}

void main(){
	setup();
	cleanup();
}
