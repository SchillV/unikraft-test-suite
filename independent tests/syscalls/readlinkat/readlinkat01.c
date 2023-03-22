#include "incl.h"
#define _GNU_SOURCE

void setup(void);

void cleanup(void);
char *TCID = "readlinkat01";

int dir_fd, fd;

int fd_invalid = 100;

int fd_atcwd = AT_FDCWD;
#define TEST_SYMLINK "readlink_symlink"
#define TEST_FILE "readlink_file"

char abspath[1024];

struct test_case {
	int *dir_fd;
	const char *path;
	const char *exp_buf;
	int exp_ret;
	int exp_errno;
} test_cases[] = {
	{&dir_fd, TEST_SYMLINK, TEST_FILE, sizeof(TEST_FILE)-1, 0},
	{&dir_fd, abspath, TEST_FILE, sizeof(TEST_FILE)-1, 0},
	{&fd, TEST_SYMLINK, NULL, -1, ENOTDIR},
	{&fd_invalid, TEST_SYMLINK, NULL, -1, EBADF},
	{&fd_atcwd, TEST_SYMLINK, TEST_FILE, sizeof(TEST_FILE)-1, 0},
};
int TST_TOTAL = ARRAY_SIZE(test_cases);

int  verify_readlinkat(struct test_case *test)
{
	char buf[1024];
	memset(buf, 0, sizeof(buf));
readlinkat(*test->dir_fd, test->path, buf, sizeof(buf));
	if (TEST_RETURN != test->exp_ret) {
		tst_resm(TFAIL | TTERRNO,
		         "readlinkat() returned %ld, expected %d",
		         TEST_RETURN, test->exp_ret);
		return;
	}
	if (TEST_ERRNO != test->exp_errno) {
		tst_resm(TFAIL | TTERRNO,
		         "readlinkat() returned %ld, expected %d",
		         TEST_RETURN, test->exp_ret);
		return;
	}
	if (test->exp_ret > 0 && strcmp(test->exp_buf, buf)) {
		tst_resm(TFAIL, "Unexpected buffer have '%s', expected '%s'",
		         buf, test->exp_buf);
		return;
	}
	tst_resm(TPASS | TTERRNO, "readlinkat() returned %ld", TEST_RETURN);
}
int main(int ac, char **av)
{
	int lc;
	int i;
	tst_parse_opts(ac, av, NULL, NULL);
	setup();
	for (lc = 0; TEST_LOOPING(lc); lc++) {
		for (i = 0; i < TST_TOTAL; i++)
int 			verify_readlinkat(&test_cases[i]);
	}
	cleanup();
	tst_exit();
}

void setup(void)
{
	tst_tmpdir();
	char *tmpdir = tst_get_tmpdir();
	snprintf(abspath, sizeof(abspath), "%s/" TEST_SYMLINK, tmpdir);
	free(tmpdir);
	fd = open(cleanup, TEST_FILE, O_CREAT, 0600);
	symlink(cleanup, TEST_FILE, TEST_SYMLINK);
	dir_fd = open(cleanup, ".", O_DIRECTORY);
	TEST_PAUSE;
}

void cleanup(void)
{
	if (fd > 0 && close(fd))
		tst_resm(TWARN | TERRNO, "Failed to close fd");
	if (dir_fd > 0 && close(dir_fd))
		tst_resm(TWARN | TERRNO, "Failed to close dir_fd");
	tst_rmdir();
}

