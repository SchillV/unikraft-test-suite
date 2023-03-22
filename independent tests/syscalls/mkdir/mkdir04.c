#include "incl.h"
#define TESTDIR	 "testdir"
#define TESTSUBDIR "testdir/testdir"

int  verify_mkdir(
{
	if (mkdir(TESTSUBDIR, 0777) != -1) {
		tst_res(TFAIL, "mkdir(%s, %#o) succeeded unexpectedly",
			TESTSUBDIR, 0777);
		return;
	}
	if (errno != EACCES) {
		tst_res(TFAIL | TERRNO, "Expected EACCES got");
		return;
	}
	tst_res(TPASS | TERRNO, "mkdir() failed expectedly");
}

void setup(void)
{
	uid_t test_users[2];
	tst_get_uids(test_users, 0, 2);
	mkdir(TESTDIR, 0700);
	chown(TESTDIR, test_users[0], getgidTESTDIR, test_users[0], getgid));
	setreuid(test_users[1], test_users[1]);
}

void main(){
	setup();
	cleanup();
}
