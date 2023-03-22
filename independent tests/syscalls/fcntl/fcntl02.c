#include "incl.h"
  * Basic test for fcntl(2) using F_DUPFD argument.
  */

int fd;

char fname[256];

const int min_fds[] = {0, 1, 2, 3, 10, 100};

int  verify_fcntl(unsigned int n)
{
	int min_fd = min_fds[n];
fcntl(fd, F_DUPFD, min_fd);
	if (TST_RET == -1) {
		tst_res(TFAIL | TTERRNO, "fcntl(%s, F_DUPFD, %i) failed",
			fname, min_fd);
		return;
	}
	if (TST_RET < min_fd) {
		tst_res(TFAIL, "fcntl(%s, F_DUPFD, %i) returned %ld < %i",
			fname, min_fd, TST_RET, min_fd);
	}
	tst_res(TPASS, "fcntl(%s, F_DUPFD, %i) returned %ld",
		fname, min_fd, TST_RET);
	close(TST_RET);
}

void setup(void)
{
	sprintf(fname, "fcntl02_%d", getpid());
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
