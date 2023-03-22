#include "incl.h"

char fifo[100];

int rfd, wfd;

long page_size;

int  verify_write()
{
	char wbuf[8 * page_size];
	int ret = write(wfd, wbuf, sizeof(wbuf));
	return -ret;
}

void setup(void)
{
	page_size = getpagesize();
	char wbuf[17 * page_size];
	sprintf(fifo, "%s.%d", fifo, getpid());
	mknod(fifo, S_IFIFO | 0777, 0);
	rfd = open(fifo, O_RDONLY | O_NONBLOCK);
	wfd = open(fifo, O_WRONLY | O_NONBLOCK);
	write(wfd, wbuf, sizeof(wbuf));
}

void cleanup(void)
{
	if (rfd > 0)
		close(rfd);
	if (wfd > 0)
		close(wfd);
}

void main(){
	setup();
	if(verify_write() > 0)
		printf("test succeeded\n");
	else
		printf("test failed\n");
	cleanup();
}
