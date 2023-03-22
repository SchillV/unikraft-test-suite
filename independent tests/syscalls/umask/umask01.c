#include "incl.h"

int  verify_umask(
{
	struct stat statbuf;
	int mskval;
	int fd;
	int failflag = 0;
	unsigned low9mode;
	for (mskval = 0000; mskval < 01000; mskval++) {
umask(mskval);
		if (TST_RET < 0 || TST_RET > 0777) {
			tst_brk(TFAIL, "umask(%o) result outside range %ld",
				mskval, TST_RET);
		}
		if (mskval > 0000 && TST_RET != mskval - 1) {
			failflag = 1;
			tst_res(TFAIL, "umask(%o) returned %ld, expected %d",
				mskval, TST_RET, mskval - 1);
		}
		fd = creat("testfile", 0777);
		close(fd);
		stat("testfile", &statbuf);
		low9mode = statbuf.st_mode & 0777;
		if (low9mode != (~mskval & 0777)) {
			failflag = 1;
			tst_res(TFAIL, "File mode got %o, expected %o",
				low9mode, ~mskval & 0777);
		}
		unlink("testfile");
	}
	if (!failflag)
		tst_res(TPASS, "All files created with correct mode");
}

void main(){
	setup();
	cleanup();
}
