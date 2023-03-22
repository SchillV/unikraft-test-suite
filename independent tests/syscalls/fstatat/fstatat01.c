#include "incl.h"
#define _GNU_SOURCE
#define TEST_CASES 6
#ifndef AT_FDCWD
#define AT_FDCWD -100
#endif
void setup();
void cleanup();
char *TCID = "fstatat01";
int TST_TOTAL = TEST_CASES;

const char pathname[] = "fstatattestdir",
		  testfile[] = "fstatattestfile.txt",
		  testfile2[] = "fstatattestdir/fstatattestfile.txt";

char *testfile3;

int fds[TEST_CASES];

const char *filenames[TEST_CASES];

const int expected_errno[] = { 0, 0, ENOTDIR, EBADF, EINVAL, 0 };

const int flags[] = { 0, 0, 0, 0, 9999, 0 };
#if !defined(HAVE_FSTATAT)
#if (__NR_fstatat64 > 0)
int fstatat(int dirfd, const char *filename, struct stat64 *statbuf, int flags)
{
	return tst_syscall(__NR_fstatat64, dirfd, filename, statbuf, flags);
}
#elif (__NR_newfstatat > 0)
int fstatat(int dirfd, const char *filename, struct stat *statbuf, int flags)
{
	return tst_syscall(__NR_newfstatat, dirfd, filename, statbuf, flags);
}
#else
int fstatat(int dirfd, const char *filename, struct stat *statbuf, int flags)
{
	return tst_syscall(__NR_fstatat, dirfd, filename, statbuf, flags);
}
#endif
#endif
int main(int ac, char **av)
{
	int lc, i;
#if !defined(HAVE_FSTATAT) && (__NR_fstatat64 > 0)
	

struct stat64 statbuf;
#else
	

struct stat statbuf;
#endif
	tst_parse_opts(ac, av, NULL, NULL);
	setup();
	for (lc = 0; TEST_LOOPING(lc); lc++) {
		tst_count = 0;
		for (i = 0; i < TST_TOTAL; i++) {
fstatat(fds[i], filenames[i], &statbuf, flags[i]);
			if (TEST_ERRNO == expected_errno[i]) {
				tst_resm(TPASS | TTERRNO,
					 "fstatat failed as expected");
			} else
				tst_resm(TFAIL | TTERRNO, "fstatat failed");
		}
	}
	cleanup();
	tst_exit();
}
void setup(void)
{
	tst_sig(NOFORK, DEF_HANDLER, cleanup);
	tst_tmpdir();
	char *abs_path = tst_get_tmpdir();
	asprintf(cleanup, &testfile3, "%s/fstatattestfile3.txt", abs_path);
	free(abs_path);
	mkdir(cleanup, pathname, 0700);
	fds[0] = open(cleanup, pathname, O_DIRECTORY);
	fds[1] = fds[4] = fds[0];
	fprintf(cleanup, testfile, testfile);
	fprintf(cleanup, testfile2, testfile2);
	fds[2] =  open(cleanup, testfile3, O_CREAT | O_RDWR, 0600);
	fds[3] = 100;
	fds[5] = AT_FDCWD;
	filenames[0] = filenames[2] = filenames[3] = filenames[4] =
	    filenames[5] = testfile;
	filenames[1] = testfile3;
	TEST_PAUSE;
}
void cleanup(void)
{
	if (fds[0] > 0)
		close(fds[0]);
	if (fds[2] > 0)
		close(fds[2]);
	free(testfile3);
	tst_rmdir();
}

