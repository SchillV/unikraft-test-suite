#include "incl.h"
#define _GNU_SOURCE
#define TEST_DIR "test_dir/"
#define TEST_DIR2 "test_dir2/"
#define TEST_FILE "test_file"
#define TEST_FILE2 "test_file2"
#define TEST_FILE3 "test_file3"
#define NON_EXIST "non_exist"
char *TCID = "renameat201";

int olddirfd;

int newdirfd;

long fs_type;

struct test_case {
	int *olddirfd;
	const char *oldpath;
	int *newdirfd;
	const char *newpath;
	int flags;
	int exp_errno;
} test_cases[] = {
	{&olddirfd, TEST_FILE, &newdirfd, TEST_FILE2, RENAME_NOREPLACE, EEXIST},
	{&olddirfd, TEST_FILE, &newdirfd, TEST_FILE2, RENAME_EXCHANGE, 0},
	{&olddirfd, TEST_FILE, &newdirfd, NON_EXIST, RENAME_EXCHANGE, ENOENT},
	{&olddirfd, TEST_FILE, &newdirfd, TEST_FILE3, RENAME_NOREPLACE, 0},
	{&olddirfd, TEST_FILE, &newdirfd, TEST_FILE2, RENAME_NOREPLACE
				| RENAME_EXCHANGE, EINVAL},
	{&olddirfd, TEST_FILE, &newdirfd, TEST_FILE2, RENAME_WHITEOUT
				| RENAME_EXCHANGE, EINVAL}
};
int TST_TOTAL = ARRAY_SIZE(test_cases);

void setup(void);

void cleanup(void);

int  renameat2_verify(const struct test_case *test);
int main(int ac, char **av)
{
	int i;
	int lc;
	tst_parse_opts(ac, av, NULL, NULL);
	setup();
	for (lc = 0; lc < TEST_LOOPING(lc); lc++) {
		tst_count = 0;
		for (i = 0; i < TST_TOTAL; i++)
int 			renameat2_verify(&test_cases[i]);
	}
	cleanup();
	tst_exit();
}

void setup(void)
{
	if ((tst_kvercmp(3, 15, 0)) < 0) {
		tst_brkm(TCONF, NULL,
			"This test can only run on kernels that are 3.15. and higher");
	}
	tst_tmpdir();
	fs_type = tst_fs_type(cleanup, ".");
	mkdir(cleanup, TEST_DIR, 0700);
	mkdir(cleanup, TEST_DIR2, 0700);
	touch(cleanup, TEST_DIR TEST_FILE, 0600, NULL);
	touch(cleanup, TEST_DIR2 TEST_FILE2, 0600, NULL);
	touch(cleanup, TEST_DIR TEST_FILE3, 0600, NULL);
	olddirfd = open(cleanup, TEST_DIR, O_DIRECTORY);
	newdirfd = open(cleanup, TEST_DIR2, O_DIRECTORY);
}

void cleanup(void)
{
	if (olddirfd > 0 && close(olddirfd) < 0)
		tst_resm(TWARN | TERRNO, "close olddirfd failed");
	if (newdirfd > 0 && close(newdirfd) < 0)
		tst_resm(TWARN | TERRNO, "close newdirfd failed");
	tst_rmdir();
}

int  renameat2_verify(const struct test_case *test)
{
renameat2(*(test->olddirfd), test->oldpat;
			*(test->newdirfd), test->newpath, test->flags));
	if ((test->flags & RENAME_EXCHANGE) && EINVAL == TEST_ERRNO
		&& fs_type == TST_BTRFS_MAGIC) {
		tst_resm(TCONF,
			"RENAME_EXCHANGE flag is not implemeted on %s",
			tst_fs_type_name(fs_type));
		return;
	}
	if (test->exp_errno && TEST_RETURN != -1) {
		tst_resm(TFAIL, "renameat2() succeeded unexpectedly");
		return;
	}
	if (test->exp_errno == 0 && TEST_RETURN != 0) {
		tst_resm(TFAIL | TTERRNO, "renameat2() failed unexpectedly");
		return;
	}
	if (test->exp_errno == TEST_ERRNO) {
		tst_resm(TPASS | TTERRNO,
		"renameat2() returned the expected value");
		return;
	}
	tst_resm(TFAIL | TTERRNO,
		"renameat2() got unexpected return value: expected: %d - %s",
			test->exp_errno, tst_strerrno(test->exp_errno));
}

