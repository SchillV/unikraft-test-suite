#include "incl.h"

int efd, sfd[2];

struct epoll_event e;

void run(void)
{
do_epoll_pwait(efd, &e, 1, -1, NULL);
	if (TST_RET == 1) {
		tst_res(TPASS, "do_epoll_pwait() succeeded");
		return;
	}
	tst_res(TFAIL, "do_epoll_pwait() returned %li, expected 1", TST_RET);
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
	write(1, sfd[1], "w", 1);
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
