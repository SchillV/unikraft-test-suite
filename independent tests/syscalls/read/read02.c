#include "incl.h"
#define _GNU_SOURCE

int badfd = -1;

int fd2, fd3, fd4 = -1;

char buf[BUFSIZ];

void *bufaddr = buf;

void *outside_buf = (void *)-1;

void *addr4;

void *addr5;

long fs_type;

struct tcase {
	int *fd;
	void **buf;
	size_t count;
	int exp_error;
} tcases[] = {
	{&badfd, &bufaddr, 1, EBADF},
	{&fd2, &bufaddr, 1, EISDIR},
#ifndef UCLINUX
	{&fd3, &outside_buf, 1, EFAULT},
#endif
	{&fd4, &addr4, 1, EINVAL},
	{&fd4, &addr5, 4096, EINVAL},
};

int  verify_read(unsigned int n)
{
	struct tcase *tc = &tcases[n];
	if (tc->fd == &fd4 && *tc->fd == -1) {
		printf("O_DIRECT not supported on this filesystem\n");
		return 0;
	}
int ret = read(*tc->fd, *tc->buf, tc->count);
	if (*tc->fd == fd4 && ret >= 0) {
		printf("O_DIRECT unaligned reads fallbacks to buffered I/O\n");
		return 1;
	}
	if (ret != -1) {
		printf("read() succeeded unexpectedly\n");
		return -1;
	}
	if (errno == tc->exp_error) {
		printf("read() failed as expected\n");
		return 1;
	} else {
		printf("read() failed unexpectedly, "
			"expected %d\n", tc->exp_error);
		return -1;
	}
}

void setup(void)
{
	fd2 = open(".", __O_DIRECTORY);
	fprintf("test_file", "A");
	fd3 = open("test_file", O_RDWR);
#if !defined(UCLINUX)
	outside_buf = mmap(0, 1, PROT_NONE,
				MAP_PRIVATE | 0x20, 0, 0);
#endif
	addr4 = memalign(getpagesize(), (4096 * 10));
	addr5 = addr4 + 1;
}

void cleanup(void)
{
	free(addr4);
	if (fd4 > 0)
		close(fd4);
	if (fd3 > 0)
		close(fd3);
	if (fd2 > 0)
		close(fd2);
}

void main(){
	setup();
	for(int i =0; i<4; i++){
		if(verify_read(i) == 0)
			printf("Invalid or broken test\n");
		else if(verify_read(i) == -1)
			printf("Test failed\n");
		else
			printf("Test succeeded\n");
	}
	cleanup();
}
