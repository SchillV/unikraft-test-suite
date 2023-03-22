#include "incl.h"

int fd;

int  verify_dup(
{
dup(fd);
	if (TST_RET < -1) {
		tst_res(TFAIL, "Invalid dup() return value %ld", TST_RET);
	} else if (TST_RET == -1) {
		tst_res(TFAIL | TTERRNO, "dup(%d) Failed", fd);
	} else {
		tst_res(TPASS, "dup(%d) returned %ld", fd, TST_RET);
		close(TST_RET);
	}
}

void setup(void)
{
	fd = open("dupfile", O_RDWR | O_CREAT, 0700);
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
