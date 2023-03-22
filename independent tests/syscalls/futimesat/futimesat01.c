#include "incl.h"
#define _GNU_SOURCE
#define TEST_CASES 5
#ifndef AT_FDCWD
#define AT_FDCWD -100
#endif
void setup();
void cleanup();
char *TCID = "futimesat01";
int TST_TOTAL = TEST_CASES;

const char pathname[] = "futimesattestdir",
		  testfile[] = "futimesattestfile.txt",
		  testfile2[] = "futimesattestdir/futimesattestfile.txt";

char *testfile3;

int fds[TEST_CASES];

const char *filenames[TEST_CASES];

const int expected_errno[] = { 0, 0, ENOTDIR, EBADF, 0 };
int myfutimesat(int dirfd, const char *filename, struct timeval *times)
{
	return tst_syscall(__NR_futimesat, dirfd, filename, times);
}
int main(int ac, char **av)
{
	int lc, i;
	struct timeval times[2];
	tst_parse_opts(ac, av, NULL, NULL);
	setup();
	for (lc = 0; TEST_LOOPING(lc); lc++) {
		tst_count = 0;
		for (i = 0; i < TST_TOTAL; i++) {
			gettimeofday(&times[0], NULL);
			gettimeofday(&times[1], NULL);
myfutimesat(fds[i], filenames[i], times);
			if (TEST_ERRNO == expected_errno[i]) {
				tst_resm(TPASS | TTERRNO,
					 "futimesat() returned expected errno");
			} else {
				tst_resm(TFAIL | TTERRNO, "futimesat() failed");
			}
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
	asprintf(cleanup, &testfile3, "%s/futimesatfile3.txt", abs_path);
	free(abs_path);
	mkdir(cleanup, pathname, 0700);
	fds[0] = open(cleanup, pathname, O_DIRECTORY);
	fds[1] = fds[0];
	fprintf(cleanup, testfile, testfile);
	fprintf(cleanup, testfile2, testfile2);
	fds[2] = open(cleanup, testfile3, O_CREAT | O_RDWR, 0600);
	fds[3] = 100;
	fds[4] = AT_FDCWD;
	filenames[0] = filenames[2] = filenames[3] = filenames[4] = testfile;
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

