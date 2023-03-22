#include "incl.h"
#define _GNU_SOURCE
#define TESTFILE	"testfile"

void setup(void);

void cleanup(void);

int dir_fd;

int fd;

int no_fd = -1;

int cu_fd = AT_FDCWD;

struct test_case_t {
	int exp_ret;
	int exp_errno;
	int flag;
	int *fds;
	char *filenames;
} test_cases[] = {
	{0, 0, 0, &dir_fd, TESTFILE},
	{-1, ENOTDIR, 0, &fd, TESTFILE},
	{-1, EBADF, 0, &no_fd, TESTFILE},
	{-1, EINVAL, 9999, &dir_fd, TESTFILE},
	{0, 0, 0, &cu_fd, TESTFILE},
};
char *TCID = "fchownat01";
int TST_TOTAL = ARRAY_SIZE(test_cases);

int  fchownat_verify(const struct test_case_t *);
int main(int ac, char **av)
{
	int lc;
	int i;
	tst_parse_opts(ac, av, NULL, NULL);
	setup();
	for (lc = 0; TEST_LOOPING(lc); lc++) {
		tst_count = 0;
		for (i = 0; i < TST_TOTAL; i++)
int 			fchownat_verify(&test_cases[i]);
	}
	cleanup();
	tst_exit();
}

void setup(void)
{
	tst_sig(NOFORK, DEF_HANDLER, cleanup);
	TEST_PAUSE;
	tst_tmpdir();
	dir_fd = open(cleanup, "./", O_DIRECTORY);
	touch(cleanup, TESTFILE, 0600, NULL);
	fd = open(cleanup, "testfile2", O_CREAT | O_RDWR, 0600);
}

int  fchownat_verify(const struct test_case_t *test)
{
fchownat(*(test->fds), test->filenames, geteuid(;
		      getegid(), test->flag));
	if (TEST_RETURN != test->exp_ret) {
		tst_resm(TFAIL | TTERRNO,
			 "fchownat() returned %ld, expected %d, errno=%d",
			 TEST_RETURN, test->exp_ret, test->exp_errno);
		return;
	}
	if (TEST_ERRNO == test->exp_errno) {
		tst_resm(TPASS | TTERRNO,
			 "fchownat() returned the expected errno %d: %s",
			 test->exp_ret, strerror(test->exp_errno));
	} else {
		tst_resm(TFAIL | TTERRNO,
			 "fchownat() failed unexpectedly; expected: %d - %s",
			 test->exp_errno, strerror(test->exp_errno));
	}
}

void cleanup(void)
{
	if (fd > 0)
		close(fd);
	if (dir_fd > 0)
		close(dir_fd);
	tst_rmdir();
}

