#include "incl.h"

char fifo[100];

int rfd, wfd;

int  verify_read()
{
	int c;
	int ret = read(rfd, &c, 1);
	if(ret == -1)
		return 1;
	else
		return -1;
}

void setup(void)
{
	struct stat buf;
	sprintf(fifo, "fifo.%d", getpid());
	mknod(fifo, S_IFIFO | 0777, 0);
	stat(fifo, &buf);
	if ((buf.st_mode & S_IFIFO) == 0){
		printf("Mode does not indicate fifo file\n");
		exit(0);
	}
	rfd = open(fifo, O_RDONLY | O_NONBLOCK);
	wfd = open(fifo, O_WRONLY | O_NONBLOCK);
}

void cleanup(void)
{
	close(rfd);
	close(wfd);
	unlink(fifo);
}

void main(){
	setup();
	if(verify_read() == 1)
		printf("test succeeded\n");
	else
		printf("test failed\n");
	cleanup();
}
