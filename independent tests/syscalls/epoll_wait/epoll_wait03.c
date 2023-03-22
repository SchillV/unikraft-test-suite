#include "incl.h"

struct epoll_event epevs[1] = {
	{.events = EPOLLOUT}
};

struct epoll_event *ev_rdonly, *ev_rdwr = epevs;

int fds[2], epfd, inv_epfd, bad_epfd = -1;

struct test_case_t {
	int *epfd;
	struct epoll_event **ev;
	int maxevents;
	int exp_errno;
	const char *desc;
} tc[] = {
	{&bad_epfd, &ev_rdwr, 1, EBADF, "epfd is not a valid fd"},
	{&inv_epfd, &ev_rdwr, 1, EINVAL, "epfd is not an epoll fd"},
	{&epfd, &ev_rdwr, -1, EINVAL, "maxevents is less than zero"},
	{&epfd, &ev_rdwr, 0, EINVAL, "maxevents is equal to zero"},
	{&epfd, &ev_rdonly, 1, EFAULT, "events has no write permissions"}
};

void setup(void)
{
	ev_rdonly = mmap(NULL, getpagesizeNULL, getpagesize), PROT_READ,
			      MAP_PRIVATE | MAP_ANONYMOUS, -1, 0);
	pipe(fds);
	epfd = epoll_create(1);
	if (epfd == -1)
		tst_brk(TBROK | TERRNO, "epoll_create()");
	epevs[0].data.fd = fds[1];
	if (epoll_ctl(epfd, EPOLL_CTL_ADD, fds[1], &epevs[0]))
		tst_brk(TBROK | TERRNO, "epoll_clt(..., EPOLL_CTL_ADD, ...)");
}

int  verify_epoll_wait(unsigned int n)
{
	TST_EXP_FAIL(epoll_wait(*tc[n].epfd, *tc[n].ev, tc[n].maxevents, -1),
		     tc[n].exp_errno, "epoll_wait() %s", tc[n].desc);
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
