#include "incl.h"

int fd[2];

int  verify_pipe(
{
	char buf[2];
pipe(fd);
	if (TST_RET == -1) {
		tst_res(TFAIL | TTERRNO, "pipe() failed unexpectedly");
		return;
	}
write(fd[0], "A", 1);
	if (TST_RET == -1 && errno == EBADF) {
		tst_res(TPASS | TTERRNO, "expected failure writing "
			"to read end of pipe");
	} else {
		tst_res(TFAIL | TTERRNO, "unexpected failure writing "
			"to read end of pipe");
	}
read(fd[1], buf, 1);
	if (TST_RET == -1 && errno == EBADF) {
		tst_res(TPASS | TTERRNO, "expected failure reading "
			"from write end of pipe");
	} else {
		tst_res(TFAIL | TTERRNO, "unexpected failure reading "
			"from write end of pipe");
	}
	close(fd[0]);
	close(fd[1]);
}

void main(){
	setup();
	cleanup();
}
