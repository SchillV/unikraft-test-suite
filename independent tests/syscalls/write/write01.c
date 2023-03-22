#include "incl.h"

int fd;

int  verify_write()
{
	int i, badcount = 0;
	char buf[BUFSIZ];
	memset(buf, 'w', BUFSIZ);
	lseek(fd, 0, SEEK_SET);
	for (i = BUFSIZ; i > 0; i--) {
int ret = write(fd, buf, i);
		if (ret == -1) {
			printf("write failed, error %d\n", errno);
			return -1;
		}
		if (ret != i) {
			badcount++;
			printf("write() returned %ld, expected %d\n",
				ret, i);
		}
	}
	if (badcount != 0){
		printf("write() failed to return proper count\n");
		return -1;
	}
	else{
		printf("write() passed\n");
		return 1;
	}
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
