#include "incl.h"
#define MAX_DEPTH 5

int epfd, new_epfd;

int fd[2];

struct epoll_event events = {.events = EPOLLIN};

void setup(void)
{
	int depth;
	pipe(fd);
	for (depth = 0, epfd = fd[0]; depth < MAX_DEPTH; depth++) {
		new_epfd = epoll_create(1);
		if (new_epfd == -1)
			tst_brk(TBROK | TERRNO, "fail to create epoll instance");
		events.data.fd = epfd;
		if (epoll_ctl(new_epfd, EPOLL_CTL_ADD, epfd, &events))
			tst_brk(TBROK | TERRNO, "epoll_clt(..., EPOLL_CTL_ADD, ...)");
		epfd = new_epfd;
	}
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
	new_epfd = epoll_create(1);
	if (new_epfd == -1)
		tst_brk(TBROK | TERRNO, "fail to create epoll instance");
	events.data.fd = epfd;
	TST_EXP_FAIL(epoll_ctl(new_epfd, EPOLL_CTL_ADD, epfd, &events), EINVAL,
		     "epoll_clt(..., EPOLL_CTL_ADD, ...) with number of nesting is 5");
	close(new_epfd);
}

void main(){
	setup();
	cleanup();
}
