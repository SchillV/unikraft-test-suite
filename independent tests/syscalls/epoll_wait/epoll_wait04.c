#include "incl.h"

int epfd, fds[2];

struct epoll_event epevs[1] = {
	{.events = EPOLLIN}
};

void run(void)
{
	tst_timer_start(CLOCK_MONOTONIC);
epoll_wait(epfd, epevs, 1, 0);
	tst_timer_stop();
	if (TST_RET != 0)
		tst_res(TFAIL | TTERRNO, "epoll_wait() returned %li", TST_RET);
	if (tst_timer_elapsed_us() <= USEC_PRECISION)
		tst_res(TPASS, "epoll_wait() returns immediately with a timeout equal to zero");
	else
		tst_res(TFAIL, "epoll_wait() waited for %llius with a timeout equal to zero",
			tst_timer_elapsed_us());
}

void setup(void)
{
	pipe(fds);
	epfd = epoll_create(1);
	if (epfd == -1)
		tst_brk(TBROK | TERRNO, "epoll_create()");
	epevs[0].data.fd = fds[0];
	if (epoll_ctl(epfd, EPOLL_CTL_ADD, fds[0], &epevs[0]))
		tst_brk(TBROK | TERRNO, "epoll_clt(..., EPOLL_CTL_ADD, ...)");
}

void cleanup(void)
{
	if (epfd > 0)
		close(epfd);
	if (fds[0] > 0)
		close(fds[0]);
	if (fds[1] > 0)
		close(fds[1]);
}

void main(){
	setup();
	cleanup();
}
