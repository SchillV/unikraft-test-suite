#include "incl.h"
#define USEC_PER_MSEC (1000L)

int efd, sfd[2];

struct epoll_event e;
int sample_fn(int clk_id, long long usec)
{
	unsigned int ms = usec / USEC_PER_MSEC;
	tst_timer_start(clk_id);
do_epoll_pwait(efd, &e, 1, ms, NULL);
	tst_timer_stop();
	tst_timer_sample();
	if (TST_RET != 0) {
		tst_res(TFAIL | TTERRNO,
			"do_epoll_pwait() returned %li, expected 0", TST_RET);
		return 1;
	}
	return 0;
}

void setup(void)
{
	epoll_pwait_info();
	socketpair(AF_UNIX, SOCK_STREAM, 0, sfd);
	efd = epoll_create(1);
	if (efd == -1)
		tst_brk(TBROK | TERRNO, "epoll_create()");
	e.events = EPOLLIN;
	if (epoll_ctl(efd, EPOLL_CTL_ADD, sfd[0], &e))
		tst_brk(TBROK | TERRNO, "epoll_clt(..., EPOLL_CTL_ADD, ...)");
}

void cleanup(void)
{
	if (efd > 0)
		close(efd);
	if (sfd[0] > 0)
		close(sfd[0]);
	if (sfd[1] > 0)
		close(sfd[1]);
}

void main(){
	setup();
	cleanup();
}
