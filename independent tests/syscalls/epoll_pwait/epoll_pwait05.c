#include "incl.h"

int efd, sfd[2];

struct epoll_event e;

struct test_case_t {
	struct timespec ts;
	int exp_errno;
	const char *desc;
} tc[] = {
	{{.tv_sec = -1}, EINVAL, "ts.tv_sec < 0"},
	{{.tv_nsec = -1}, EINVAL, "ts.tv_nsec < 0"},
	{{.tv_nsec = NSEC_PER_SEC}, EINVAL, "ts.tv_nsec >= NSEC_PER_SEC"},
};

void run_all(unsigned int n)
{
	TST_EXP_FAIL(epoll_pwait2(efd, &e, 1, &tc[n].ts, NULL),
		     tc[n].exp_errno, "with %s", tc[n].desc);
}

void setup(void)
{
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
