#include "incl.h"

fd_set readfds_pipe, writefds_pipe;

int fd_empty[2], fd_full[2];

struct tcases {
	int *fd;
	fd_set *readfds;
	fd_set *writefds;
	char *desc;
} tests[] = {
	{&fd_empty[0], &readfds_pipe, NULL, "No data to read"},
	{&fd_full[1], NULL, &writefds_pipe, "No space to write"},
};

void run(unsigned int n)
{
	struct tcases *tc = &tests[n];
	struct timeval timeout = {.tv_sec = 0, .tv_usec = 1000};
	FD_SET(fd_empty[0], &readfds_pipe);
	FD_SET(fd_full[1], &writefds_pipe);
do_select(*tc->fd + 1, tc->readfds, tc->writefds, NULL, &timeout);
	if (TST_RET) {
		tst_res(TFAIL, "%s: select() should have timed out", tc->desc);
		return;
	}
	if ((tc->readfds && FD_ISSET(*tc->fd, tc->readfds)) ||
	    (tc->writefds && FD_ISSET(*tc->fd, tc->writefds))) {
		tst_res(TFAIL, "%s: select() didn't clear the fd set", tc->desc);
		return;
	}
	tst_res(TPASS, "%s: select() cleared the fd set", tc->desc);
}

void setup(void)
{
	int buf = 0;
	select_info();
	pipe(fd_empty);
	FD_ZERO(&readfds_pipe);
	pipe2(fd_full, O_NONBLOCK);
	FD_ZERO(&writefds_pipe);
	do {
write(fd_full[1], &buf, sizeof(buf));
	} while (TST_RET != -1);
	if (TST_ERR != EAGAIN)
		tst_res(TFAIL | TTERRNO, "write() failed with unexpected error");
}

void main(){
	setup();
	cleanup();
}
