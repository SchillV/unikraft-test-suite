#include "incl.h"
#define MNT_POINT "mntpoint"
#define DIR1 "dir1"
#define DIR2 "dir2"
#define TEMP_FILE DIR2"/tmpfile"

void setup(void)
{
	chdir(MNT_POINT);
	mkdir(DIR1, 00770);
	mkdir(DIR2, 00770);
	touch(TEMP_FILE, 0700, NULL);
}

void run(void)
{
rename(DIR1, DIR2);
	if (TST_RET == -1 && (TST_ERR == ENOTEMPTY || TST_ERR == EEXIST))
		tst_res(TPASS | TTERRNO, "rename() failed as expected");
	else if (TST_RET == 0)
		tst_res(TFAIL, "rename() succeeded unexpectedly");
	else
		tst_res(TFAIL | TTERRNO, "rename() failed, but not with expected errno");
}

void main(){
	setup();
	cleanup();
}
