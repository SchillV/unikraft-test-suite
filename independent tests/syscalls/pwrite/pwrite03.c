#include "incl.h"

int fd;

int  verify_pwrite(
{
	TST_EXP_PASS(pwrite(fd, NULL, 0, 0), "pwrite(%d, NULL, 0) == 0", fd);
}

void setup(void)
{
	fd = open("test_file", O_RDWR | O_CREAT, 0700);
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
