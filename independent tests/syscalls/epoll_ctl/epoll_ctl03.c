#include "incl.h"
#define NUM_EPOLL_EVENTS 8
#define WIDTH_EPOLL_EVENTS 256

int epfd, fds[2];

struct epoll_event events = {.events = EPOLLIN};

unsigned int events_type[NUM_EPOLL_EVENTS] = {
		EPOLLIN, EPOLLOUT, EPOLLPRI, EPOLLERR,
		EPOLLHUP, EPOLLET, EPOLLONESHOT, EPOLLRDHUP
};

void run_all(void)
{
	unsigned int i, j, events_bitmap;
	for (i = 0; i < WIDTH_EPOLL_EVENTS; i++) {
		events_bitmap = 0;
		for (j = 0; j < NUM_EPOLL_EVENTS; j++)
			events_bitmap |= (events_type[j] * TST_IS_BIT_SET(i, j));
		events.events = events_bitmap;
		TST_EXP_PASS(epoll_ctl(epfd, EPOLL_CTL_MOD, fds[0], &events),
			     "epoll_ctl(..., EPOLL_CTL_MOD, ...) with events.events=%08x",
			     events.events);
	}
}

void setup(void)
{
	epfd = epoll_create(1);
	if (epfd == -1)
		tst_brk(TBROK | TERRNO, "fail to create epoll instance");
	pipe(fds);
	events.data.fd = fds[0];
	if (epoll_ctl(epfd, EPOLL_CTL_ADD, fds[0], &events))
		tst_brk(TBROK | TERRNO, "epoll_clt(..., EPOLL_CTL_ADD, ...)");
}

void cleanup(void)
{
	if (epfd)
		close(epfd);
	if (fds[0])
		close(fds[0]);
	if (fds[1])
		close(fds[1]);
}

void main(){
	setup();
	cleanup();
}
