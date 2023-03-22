#include "incl.h"

int fd;

int inv_fd = -1;

char b[32];

char *buf = b;

char *bad_addr;

int pipefd[2];

struct tcase {
	int *fd;
	char **buf;
	size_t size;
	int exp_errno;
} tcases[] = {
	{&inv_fd, &buf, sizeof(buf), EBADF},
	{&fd, &bad_addr, sizeof(buf), EFAULT},
	{&pipefd[1], &buf, sizeof(buf), EPIPE},
};

int sigpipe_cnt;

void sighandler(int sig)
{
	if (sig == SIGPIPE)
		sigpipe_cnt++;
}

int  verify_write(unsigned int i)
{
	struct tcase *tc = &tcases[i];
	sigpipe_cnt = 0;
int ret=write(*tc->fd, *tc->buf, tc->size);
	if (ret != -1) {
		printf("write() succeeded unexpectedly\n");
		return -1;
	}
	if (errno != tc->exp_errno) {
		printf(
			"write() failed unexpectedly, expected %d\n",
			tc->exp_errno);
		return -1;
	}
	if (tc->exp_errno == EPIPE && sigpipe_cnt != 1) {
		printf("sigpipe_cnt = %i", sigpipe_cnt);
		return -1;
	}
	printf("write() failed expectedly\n");
	return 1;
}

void setup(void)
{
	fd = open("write_test", O_RDWR | O_CREAT, 0644);
	bad_addr = mmap(0, 1, PROT_NONE,
			MAP_PRIVATE | 0x20, 0, 0);
	pipe(pipefd);
	close(pipefd[0]);
	signal(SIGPIPE, sighandler);
}

void cleanup(void)
{
	if (fd > 0)
		close(fd);
	munmap(bad_addr, 1);
}

void main(){
	setup();
	for(int i = 0; i<3; i++){
		if(verify_write(i) == 1)
			printf("test succeeded\n");
		else
			printf("test failed\n");
	}
	cleanup();
}
