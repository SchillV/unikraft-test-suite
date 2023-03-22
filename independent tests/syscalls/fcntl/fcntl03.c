#include "incl.h"
  * Basic test for fcntl(2) using F_GETFD argument.
  */

char fname[255];

int fd;

int  verify_fcntl(
{
fcntl(fd, F_GETFD, 0);
	if (TST_RET == -1) {
		tst_res(TFAIL | TTERRNO, "fcntl(%s, F_GETFD, 0) failed",
			fname);
		return;
	}
	tst_res(TPASS, "fcntl(%s, F_GETFD, 0) returned %ld",
		fname, TST_RET);
}

void setup(void)
{
	sprintf(fname, "fcntl03_%d", getpid());
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
