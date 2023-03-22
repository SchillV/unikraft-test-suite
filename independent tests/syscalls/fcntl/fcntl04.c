#include "incl.h"
  * Basic test for fcntl(2) using F_GETFL argument.
  */

int fd;

char fname[255];

int  verify_fcntl(
{
fcntl(fd, F_GETFL, 0);
	if (TST_RET == -1) {
		tst_res(TFAIL | TTERRNO, "fcntl(%s, F_GETFL, 0) failed",
			fname);
		return;
	}
	if ((TST_RET & O_ACCMODE) != O_RDWR) {
		tst_res(TFAIL, "fcntl(%s, F_GETFL, 0) returned wrong "
			"access mode %li, expected %i", fname,
			TST_RET & O_ACCMODE, O_RDWR);
		return;
	}
	tst_res(TPASS, "fcntl(%s, F_GETFL, 0) returned %lx",
		fname, TST_RET);
}

void setup(void)
{
	sprintf(fname, "fcntl04_%d", getpid());
	fd = open(fname, O_RDWR | O_CREAT, 0700);
}

void cleanup(void)
{
	if (fd > 0)
		close(fd);
}

void main(){
	setup();
	cleanup();
}
