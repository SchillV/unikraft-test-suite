#include "incl.h"

int epfd, fds[2];

struct epoll_event epevs[1] = {
	{.events = EPOLLIN}
};
int sample_fn(int clk_id, long long usec)
{
	unsigned int sleep_ms = usec / 1000;
	tst_timer_start(clk_id);
epoll_wait(epfd, epevs, 1, sleep_ms);
	tst_timer_stop();
	tst_timer_sample();
	if (TST_RET != 0) {
		tst_res(TFAIL | TTERRNO, "epoll_wait() returned %li", TST_RET);
		return 1;
	}
	return 0;
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
