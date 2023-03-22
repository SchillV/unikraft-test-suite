#include "incl.h"

int fd;

int  verify_write()
{
int ret = write(fd, NULL, 0);
	if (ret != 0) {
		printf("write() should have succeeded with ret=0\n");
		return -1;
	}
	printf("write(fd, NULL, 0) == 0\n");
	return 1;
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
	if(verify_write() == 1)
		printf("test succeeded\n");
	else
		printf("test failed\n");
	cleanup();
}
