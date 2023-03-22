#include "incl.h"

char fname[255];

int fd;
#define BUF "davef"

int  verify_fsync(
{
	unsigned int i;
	for (i = 0; i < 10; i++) {
		write(1, fd, BUF, sizeof1, fd, BUF, sizeofBUF));
fsync(fd);
		if (TST_RET == -1)
			tst_res(TFAIL | TTERRNO, "fsync failed");
		else
			tst_res(TPASS, "fsync() returned %ld", TST_RET);
	}
}

void setup(void)
{
	sprintf(fname, "mntpoint/tfile_%d", getpid());
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
