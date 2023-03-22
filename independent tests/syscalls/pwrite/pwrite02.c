#include "incl.h"
#define TEMPFILE "pwrite_file"
#define BS 1024

int fd;

int fd_ro;

int inv_fd = -1;

int pipe_fds[2];

char buf[BS];

struct tcase {
	void *buf;
	size_t size;
	int *fd;
	off_t off;
	int exp_errno;
} tcases[] = {
	{buf, sizeof(buf), &pipe_fds[1], 0, ESPIPE},
	{buf, sizeof(buf), &fd, -1, EINVAL},
	{buf, sizeof(buf), &inv_fd, 0, EBADF},
	{buf, sizeof(buf), &fd_ro, 0, EBADF},
	{NULL, sizeof(buf), &fd, 0, EFAULT},
};

void sighandler(int sig)
{
	int ret;
	if (sig != SIGXFSZ) {
		ret = write(STDOUT_FILENO, "get wrong signal\n",
			sizeof("get wrong signal\n"));
	} else {
		ret = write(STDOUT_FILENO, "caught SIGXFSZ\n",
			sizeof("caught SIGXFSZ\n"));
	}
	(void)ret;
}

int  verify_pwrite(unsigned int i)
{
	struct tcase *tc = &tcases[i];
	TST_EXP_FAIL2(pwrite(*tc->fd, tc->buf, BS, tc->off), tc->exp_errno,
		"pwrite(%d, %d, %ld)", *tc->fd, BS, tc->off);
}

void setup(void)
{
	signal(SIGXFSZ, sighandler);
	pipe(pipe_fds);
	fd = open(TEMPFILE, O_RDWR | O_CREAT, 0666);
	fd_ro = open(TEMPFILE, O_RDONLY | O_CREAT, 0666);
}

void cleanup(void)
{
	if (fd > 0)
		close(fd);
	if (fd_ro > 0)
		close(fd_ro);
	if (pipe_fds[0] > 0)
		close(pipe_fds[0]);
	if (pipe_fds[1] > 0)
		close(pipe_fds[1]);
}

void main(){
	setup();
	cleanup();
}
