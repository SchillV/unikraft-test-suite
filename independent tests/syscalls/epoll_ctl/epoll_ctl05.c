#include "incl.h"
#define MAX_DEPTH 5

int epfd, origin_epfd, new_epfd;

int fd[2];

struct epoll_event events = {.events = EPOLLIN};

void setup(void)
{
	int i;
	pipe(fd);
	for (i = 0, epfd = fd[0]; i < MAX_DEPTH; i++, epfd = new_epfd) {
		new_epfd = epoll_create(1);
		if (new_epfd == -1)
			tst_brk(TBROK | TERRNO, "fail to create epoll instance");
		if (i == 0)
			origin_epfd = new_epfd;
		events.data.fd = epfd;
		if (epoll_ctl(new_epfd, EPOLL_CTL_ADD, epfd, &events))
			tst_brk(TBROK | TERRNO, "epoll_clt(..., EPOLL_CTL_ADD, ...)");
	}
	events.data.fd = fd[0];
	if (epoll_ctl(origin_epfd, EPOLL_CTL_DEL, fd[0], &events))
		tst_brk(TBROK | TERRNO, "epoll_clt(..., EPOLL_CTL_DEL, ...)");
}

void cleanup(void)
{
	if (fd[0])
		close(fd[0]);
	if (fd[1])
		close(fd[1]);
}

int  verify_epoll_ctl(
{
	events.data.fd = epfd;
	TST_EXP_FAIL(epoll_ctl(origin_epfd, EPOLL_CTL_ADD, epfd, &events),
		     ELOOP, "epoll_clt(..., EPOLL_CTL_ADD, ...)");
}

void main(){
	setup();
	cleanup();
}
