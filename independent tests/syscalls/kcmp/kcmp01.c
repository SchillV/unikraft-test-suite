#include "incl.h"
#define _GNU_SOURCE
#define TEST_FILE "test_file"
#define TEST_FILE2 "test_file2"

int fd1;

int fd2;

int fd3;

int pid1;

int pid2;

struct test_case {
	int *pid1;
	int *pid2;
	int type;
	int *fd1;
	int *fd2;
	int exp_different;
} test_cases[] = {
	{&pid1, &pid1, KCMP_FILE, &fd1, &fd1, 0},
	{&pid2, &pid2, KCMP_FILE, &fd1, &fd2, 0},
	{&pid1, &pid2, KCMP_FILE, &fd1, &fd1, 0},
	{&pid1, &pid2, KCMP_FILE, &fd1, &fd2, 0},
	{&pid1, &pid2, KCMP_FILE, &fd1, &fd3, 1},
};

void setup(void)
{
	fd1 = open(TEST_FILE, O_CREAT | O_RDWR | O_TRUNC, 0666);
}

void cleanup(void)
{
	if (fd1 > 0)
		close(fd1);
}

void do_child(const struct test_case *test)
{
	pid2 = getpid();
	fd3 = open(TEST_FILE2, O_CREAT | O_RDWR, 0666);
	fd2 = dup(fd1);
	if (fd2 == -1) {
		tst_res(TFAIL | TERRNO, "dup() failed unexpectedly");
		close(fd3);
		return;
	}
kcmp(*(test->pid1), *(test->pid2), test->typ;
		  *(test->fd1), *(test->fd2)));
	close(fd2);
	close(fd3);
	if (TST_RET == -1) {
		tst_res(TFAIL | TTERRNO, "kcmp() failed unexpectedly");
		return;
	}
	if ((test->exp_different && TST_RET == 0)
		|| (test->exp_different == 0 && TST_RET)) {
		tst_res(TFAIL, "kcmp() returned %lu instead of %d",
			TST_RET, test->exp_different);
		return;
	}
	tst_res(TPASS, "kcmp() returned the expected value");
}

int  verify_kcmp(unsigned int n)
{
	struct test_case *tc = &test_cases[n];
	pid1 = getpid();
	pid2 = fork();
	if (!pid2)
		do_child(tc);
}

void main(){
	setup();
	cleanup();
}
