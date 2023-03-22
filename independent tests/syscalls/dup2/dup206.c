#include "incl.h"

int fd = -1;

int  verify_dup2(
{
	TST_EXP_FD_SILENT(dup2(fd, fd), "dup2(%d, %d)", fd, fd);
	if (TST_RET != fd) {
		tst_res(TFAIL, "dup2(%d, %d) returns wrong newfd(%ld)", fd, fd, TST_RET);
		close(TST_RET);
		return;
	}
	tst_res(TPASS, "dup2(%d, %d) returns newfd(%d)", fd, fd, fd);
}

void setup(void)
{
	fd = open("testfile", O_RDWR | O_CREAT, 0666);
}

void cleanup(void)
{
	if (fd > -1)
		close(fd);
}

void main(){
	setup();
	cleanup();
}
