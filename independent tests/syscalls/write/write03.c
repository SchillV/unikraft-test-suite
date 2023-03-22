#include "incl.h"

char *bad_addr;

char wbuf[BUFSIZ], rbuf[BUFSIZ];

int fd;

int  verify_write()
{
	fd = creat("testfile", 0644);
	write(fd, wbuf, 100);
	if (write(fd, bad_addr, 100) != -1) {
		printf("write() failed to fail\n");
		close(fd);
		return -1;
	}
	close(fd);
	fd = open("testfile", O_RDONLY);
	memset(rbuf, 0, BUFSIZ);
	read(0, fd, rbuf, 100);
	if (memcmp(wbuf, rbuf, 100) == 0){
		printf("failure of write() did not corrupt the file\n");
		close(fd);
		return 1;
	}
	else{
		printf("failure of write() corrupted the file\n");
		close(fd);
		return -1;
	}
}

void setup(void)
{
	bad_addr = mmap(0, 1, PROT_NONE,
			MAP_PRIVATE | MAP_ANONYMOUS, 0, 0);
	memset(wbuf, '0', 100);
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
