#include "incl.h"
#define SIZE 512

int fd;

char buf[SIZE];

int  verify_read(void)
{
	lseek(fd, 0, SEEK_SET);
	int ret = read(fd, buf, SIZE);
	if (ret == -1)
		printf("read(2) failed\n");
	else
		printf("read(2) returned %d\n", ret);
	return ret;
}

void setup(void)
{
	memset(buf, '*', SIZE);
	fd = open("testfile", O_RDWR | O_CREAT, 0700);
	write(1, fd, buf, SIZE);
}

void cleanup(void)
{
	if (fd > 0)
		close(fd);
}

void main(){
	setup();
	if(verify_read() == -1)
		printf("test failed\n");
	else
		printf("test succeeded\n");
	cleanup();
}
