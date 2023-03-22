#include "incl.h"
#define DIR_MODE    0777
#define NOEXCUTE_MODE 0766
#define TESTDIR     "testdir"
#define TESTDIR2    "testdir/testdir2"
#define TESTDIR3    "testdir3"
#define TESTDIR4    "testdir3/testdir4"

struct testcase {
	mode_t dir_mode;
	char *subdir;
} tcases[] =  {
	{DIR_MODE | S_ISVTX, TESTDIR2},
	{NOEXCUTE_MODE, TESTDIR4},
};

void do_rmdir(unsigned int n)
{
	struct testcase *tc = &tcases[n];
rmdir(tc->subdir);
	if (TST_RET != -1) {
		tst_res(TFAIL, "rmdir() succeeded unexpectedly");
		return;
	}
	if (TST_ERR != EACCES) {
		if (tc->dir_mode & S_ISVTX && TST_ERR == EPERM)
			tst_res(TPASS | TTERRNO, "rmdir() got expected errno");
		else
			tst_res(TFAIL | TTERRNO, "expected EPERM, but got");
		return;
	}
	tst_res(TPASS | TTERRNO, "rmdir() got expected errno");
}

void setup(void)
{
	struct passwd *pw;
	pw = getpwnam("nobody");
	umask(0);
	mkdir(TESTDIR, DIR_MODE | S_ISVTX);
	mkdir(TESTDIR2, DIR_MODE);
	mkdir(TESTDIR3, NOEXCUTE_MODE);
	mkdir(TESTDIR4, DIR_MODE);
	seteuid(pw->pw_uid);
}

void cleanup(void)
{
	seteuid(0);
}

void main(){
	setup();
	cleanup();
}
