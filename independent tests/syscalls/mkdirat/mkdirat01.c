#include "incl.h"
#define _GNU_SOURCE

void setup(void);

void cleanup(void);

char relpath[256];

char abspath[1024];

int dir_fd, fd;

int fd_invalid = 100;

int fd_atcwd = AT_FDCWD;

struct test_case {
	int *dir_fd;
	const char *name;
	int exp_ret;
	int exp_errno;
} test_cases[] = {
	{&dir_fd, relpath, 0, 0},
	{&dir_fd, abspath, 0, 0},
	{&fd_atcwd, relpath, 0, 0},
	{&fd, relpath, -1, ENOTDIR},
	{&fd_invalid, relpath, -1, EBADF},
};
char *TCID = "mkdirat01";
int TST_TOTAL = ARRAY_SIZE(test_cases);

int  verify_mkdirat(struct test_case *test)
{
mkdirat(*test->dir_fd, test->name, 0600);
	if (TEST_RETURN != test->exp_ret) {
		tst_resm(TFAIL | TTERRNO,
		         "mkdirat() returned %ld, expected %d",
			 TEST_RETURN, test->exp_ret);
		return;
	}
	if (TEST_ERRNO != test->exp_errno) {
		tst_resm(TFAIL | TTERRNO,
		         "mkdirat() returned wrong errno, expected %d",
			 test->exp_errno);
		return;
	}
	tst_resm(TPASS | TTERRNO, "mkdirat() returned %ld", TEST_RETURN);
}

void setup_iteration(int i)
{
	

char testdir[256];
	char *tmpdir = tst_get_tmpdir();
	sprintf(testdir, "mkdirattestdir%d_%d", getpid(), i);
	sprintf(relpath, "mkdiratrelpath%d_%d", getpid(), i);
	sprintf(abspath, "%s/mkdiratrelpath%d_%d_2", tmpdir, getpid(), i);
	free(tmpdir);
	mkdir(cleanup, testdir, 0700);
	dir_fd = open(cleanup, testdir, O_DIRECTORY);
}

void cleanup_iteration(void)
{
	close(cleanup, dir_fd);
}
int main(int ac, char **av)
{
	int lc;
	int i;
	tst_parse_opts(ac, av, NULL, NULL);
	setup();
	for (lc = 0; TEST_LOOPING(lc); lc++) {
		tst_count = 0;
		setup_iteration(lc);
		for (i = 0; i < TST_TOTAL; i++)
int 			verify_mkdirat(test_cases + i);
		cleanup_iteration();
	}
	cleanup();
	tst_exit();
}

void setup(void)
{
	TEST_PAUSE;
	tst_tmpdir();
	fd = open(cleanup, "mkdirattestfile", O_CREAT | O_RDWR, 0600);
}

void cleanup(void)
{
	if (fd > 0)
		close(fd);
	tst_rmdir();
}

