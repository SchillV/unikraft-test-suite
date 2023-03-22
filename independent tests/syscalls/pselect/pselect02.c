#include "incl.h"

fd_set read_fds;

struct timespec time_buf;

struct tcase {
	int nfds;
	fd_set *readfds;
	struct timespec *timeout;
	int exp_errno;
} tcases[] = {
	{128, &read_fds, NULL, EBADF},
	{-1, NULL, NULL, EINVAL},
	{128, NULL, &time_buf, EINVAL},
};

void setup(void)
{
	int fd;
	fd = open("test_file", O_RDWR | O_CREAT, 0777);
	FD_ZERO(&read_fds);
	FD_SET(fd, &read_fds);
	close(fd);
	time_buf.tv_sec = -1;
	time_buf.tv_nsec = 0;
}

int  pselect_verify(unsigned int i)
{
	struct tcase *tc = &tcases[i];
	TST_EXP_FAIL(pselect(tc->nfds, tc->readfds, NULL, NULL, tc->timeout, NULL),
			tc->exp_errno, "pselect(%i, %p, %p)",
			tc->nfds, tc->readfds, tc->timeout);
}

void main(){
	setup();
	cleanup();
}
