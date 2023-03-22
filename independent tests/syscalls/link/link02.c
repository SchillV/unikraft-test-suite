#include "incl.h"
#define OLDPATH "oldpath"
#define NEWPATH "newpath"

int  verify_link(
{
	struct stat fbuf, lbuf;
	TST_EXP_PASS(link(OLDPATH, NEWPATH));
	if (!TST_PASS)
		return;
	stat(OLDPATH, &fbuf);
	stat(NEWPATH, &lbuf);
	if (fbuf.st_nlink > 1 && fbuf.st_nlink == lbuf.st_nlink) {
		tst_res(TPASS, "link("OLDPATH","NEWPATH") "
                                 "returned 0 and stat link counts match");
	} else {
		tst_res(TFAIL, "link("OLDPATH","NEWPATH") returned 0"
                                 " but stat link counts do not match %d %d",
                                 (int)fbuf.st_nlink, (int)lbuf.st_nlink);
	}
	unlink(NEWPATH);
}

void setup(void)
{
	touch(OLDPATH, 0700, NULL);
}

void main(){
	setup();
	cleanup();
}
