#include "incl.h"
#define TESTDIR "testdir"

int  verify_rmdir(
{
	struct stat buf;
	mkdir(TESTDIR, 0777);
rmdir(TESTDIR);
	if (TST_RET == -1) {
		tst_res(TFAIL | TTERRNO, "rmdir(%s) failed", TESTDIR);
		return;
	}
	if (!stat(TESTDIR, &buf))
		tst_res(TFAIL, "rmdir(%s) failed", TESTDIR);
	else
		tst_res(TPASS, "rmdir(%s) success", TESTDIR);
}

void main(){
	setup();
	cleanup();
}
