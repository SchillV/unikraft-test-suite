#include "incl.h"
#define _GNU_SOURCE
#define PATHNAME "mknodattestdir"

void setup(void);

void cleanup(void);

void clean(void);

char testfilepath[256];

char testfile[256];

char testfile2[256];

char testfile3[256];

int dir_fd, fd;

int fd_invalid = 100;

int fd_atcwd = AT_FDCWD;

struct test_case {
	int *dir_fd;
	const char *name;
	int exp_ret;
	int exp_errno;
} test_cases[] = {
	{&dir_fd, testfile, 0, 0},
	{&dir_fd, testfile3, 0, 0},
	{&fd, testfile2, -1, ENOTDIR},
	{&fd_invalid, testfile, -1, EBADF},
	{&fd_atcwd, testfile, 0, 0}
};
char *TCID = "mknodat01";
int TST_TOTAL = ARRAY_SIZE(test_cases);

dev_t dev;

int  verify_mknodat(struct test_case *test)
{
mknodat(*test->dir_fd, test->name, S_IFREG, dev);
	if (TEST_RETURN != test->exp_ret) {
		tst_resm(TFAIL | TTERRNO,
		         "mknodat() returned %ld, expected %d",
			 TEST_RETURN, test->exp_ret);
		return;
	}
	if (TEST_ERRNO != test->exp_errno) {
		tst_resm(TFAIL | TTERRNO,
		         "mknodat() returned wrong errno, expected %d",
			 test->exp_errno);
		return;
	}
	tst_resm(TPASS | TTERRNO, "mknodat() returned %ld", TEST_RETURN);
}
int main(int ac, char **av)
{
	int lc;
	int i;
	tst_parse_opts(ac, av, NULL, NULL);
	setup();
	for (lc = 0; TEST_LOOPING(lc); lc++) {
		tst_count = 0;
		for (i = 0; i < TST_TOTAL; i++)
int 			verify_mknodat(test_cases + i);
		clean();
	}
	cleanup();
	tst_exit();
}

void setup(void)
{
	char *tmpdir;
	tst_sig(NOFORK, DEF_HANDLER, cleanup);
	TEST_PAUSE;
	tst_tmpdir();
	tmpdir = tst_get_tmpdir();
	sprintf(testfilepath, PATHNAME"/mknodattestfile%d", getpid());
	sprintf(testfile, "mknodattestfile%d", getpid());
	sprintf(testfile2, "mknodattestfile2%d", getpid());
	sprintf(testfile3, "%s/mknodattestfile3%d", tmpdir, getpid());
	free(tmpdir);
	mkdir(cleanup, PATHNAME, 0700);
	dir_fd = open(cleanup, PATHNAME, O_DIRECTORY);
	fd = open(cleanup, testfile2, O_CREAT | O_RDWR, 0600);
}

void clean(void)
{
	unlink(cleanup, testfilepath);
	unlink(cleanup, testfile3);
	unlink(cleanup, testfile);
}

void cleanup(void)
{
	if (dir_fd > 0 && close(dir_fd))
		tst_resm(TWARN | TERRNO, "Failed to close(dir_fd)");
	if (fd > 0 && close(fd))
		tst_resm(TWARN | TERRNO, "Failed to close(fd)");
	tst_rmdir();
}

