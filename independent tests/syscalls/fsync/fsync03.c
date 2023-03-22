#include "incl.h"
#define FIFO_PATH "fifo"

int fifo_rfd, fifo_wfd;

int pipe_fd[2];

int sock_fd, bad_fd = -1;

const struct test_case {
	int *fd;
	int error;
} testcase_list[] = {
	{&pipe_fd[1], EINVAL},
	{&sock_fd, EINVAL},
	{&pipe_fd[0], EBADF},
	{&bad_fd, EBADF},
	{&fifo_wfd, EINVAL},
};

void setup(void)
{
	mkfifo(FIFO_PATH, 0644);
	pipe(pipe_fd);
	fifo_rfd = open(FIFO_PATH, O_RDONLY | O_NONBLOCK);
	fifo_wfd = open(FIFO_PATH, O_WRONLY);
	sock_fd = socket(AF_UNIX, SOCK_STREAM, 0);
	close(pipe_fd[0]);
}

void test_fsync(unsigned int n)
{
	const struct test_case *tc = testcase_list + n;
fsync(*tc->fd);
	if (TST_RET != -1) {
		tst_res(TFAIL, "fsync() returned unexpected value %ld",
			TST_RET);
	} else if (TST_ERR != tc->error) {
		tst_res(TFAIL | TTERRNO, "fsync(): unexpected error");
	} else {
		tst_res(TPASS | TTERRNO, "fsync() failed as expected");
	}
}

void cleanup(void)
{
	close(fifo_wfd);
	close(fifo_rfd);
	close(pipe_fd[1]);
	close(sock_fd);
}

void main(){
	setup();
	cleanup();
}
