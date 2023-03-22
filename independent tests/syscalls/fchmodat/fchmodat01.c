#include "incl.h"
#define _GNU_SOURCE
#ifndef AT_FDCWD
#define AT_FDCWD -100
#endif

char pathname[256];

char testfile[256];

char testfile2[256];

char testfile3[256];

struct tcase {
	int exp_errno;
	char *exp_errval;
} tcases[] = {
	{ 0, NULL},
	{ 0, NULL},
	{ ENOTDIR, "ENOTDIR"},
	{ EBADF, "EBADF"},
	{ 0, NULL},
	{ 0, NULL},
};

int fds[ARRAY_SIZE(tcases)];

char *filenames[ARRAY_SIZE(tcases)];

int  verify_fchmodat(unsigned int i)
{
	struct tcase *tc = &tcases[i];
	if (tc->exp_errno == 0)
		TST_EXP_PASS(tst_syscall(__NR_fchmodat, fds[i], filenames[i], 0600),
			     "fchmodat() returned the expected errno %d: %s",
			     TST_ERR, strerror(TST_ERR));
	else
		TST_EXP_FAIL(tst_syscall(__NR_fchmodat, fds[i], filenames[i], 0600),
			     tc->exp_errno,
			     "fchmodat() returned the expected errno %d: %s",
			     TST_ERR, strerror(TST_ERR));
}

void setup(void)
{
	char *abs_path = tst_get_tmpdir();
	int p = getpid();
	sprintf(pathname, "fchmodattestdir%d", p);
	sprintf(testfile, "fchmodattest%d.txt", p);
	sprintf(testfile2, "%s/fchmodattest%d.txt", abs_path, p);
	sprintf(testfile3, "fchmodattestdir%d/fchmodattest%d.txt", p, p);
	free(abs_path);
	mkdir(pathname, 0700);
	fds[0] = open(pathname, O_DIRECTORY);
	fds[1] = fds[4] = fds[0];
	fprintf(testfile, "%s", testfile);
	fprintf(testfile2, "%s", testfile2);
	fds[2] = open(testfile3, O_CREAT | O_RDWR, 0600);
	fds[3] = 100;
	fds[5] = AT_FDCWD;
	filenames[0] = filenames[2] = filenames[3] = filenames[4] = testfile;
	filenames[1] = testfile2;
	filenames[5] = testfile3;
}

void cleanup(void)
{
	if (fds[0] > 0)
		close(fds[0]);
	if (fds[2] > 0)
		close(fds[2]);
}

void main(){
	setup();
	cleanup();
}
