#include "incl.h"

int epfd;

int fd[2], unsupported_fd;

int inv = -1;

struct epoll_event events[2] = {
	{.events = EPOLLIN},
	{.events = EPOLLOUT},
};

struct testcase {
	int *epfd;
	int opt;
	int *fd;
	struct epoll_event *event;
	int exp_err;
	const char *desc;
} tc[] = {
	{&inv, EPOLL_CTL_ADD, &fd[1], &events[1], EBADF, "epfd is an invalid fd"},
	{&epfd, EPOLL_CTL_ADD, &unsupported_fd, &events[1], EPERM, "fd does not support epoll"},
	{&epfd, EPOLL_CTL_ADD, &inv, &events[1], EBADF, "fd is an invalid fd"},
	{&epfd, -1, &fd[1], &events[1], EINVAL, "op is not supported"},
	{&epfd, EPOLL_CTL_ADD, &epfd, &events[1], EINVAL, "fd is the same as epfd"},
	{&epfd, EPOLL_CTL_ADD, &fd[1], NULL, EFAULT, "events is NULL"},
	{&epfd, EPOLL_CTL_DEL, &fd[1], &events[1], ENOENT, "fd is not registered with EPOLL_CTL_DEL"},
	{&epfd, EPOLL_CTL_MOD, &fd[1], &events[1], ENOENT, "fd is not registered with EPOLL_CTL_MOD"},
	{&epfd, EPOLL_CTL_ADD, &fd[0], &events[0], EEXIST, "fd is already registered with EPOLL_CTL_ADD"}
};

void setup(void)
{
	unsupported_fd = open(".", O_RDONLY|O_DIRECTORY, 0);
	epfd = epoll_create(2);
	if (epfd == -1)
		tst_brk(TBROK | TERRNO, "fail to create epoll instance");
	pipe(fd);
	events[0].data.fd = fd[0];
	events[1].data.fd = fd[1];
	if (epoll_ctl(epfd, EPOLL_CTL_ADD, fd[0], &events[0]))
		tst_brk(TBROK | TERRNO, "epoll_clt(..., EPOLL_CTL_ADD, ...)");
}

void cleanup(void)
{
	if (epfd)
		close(epfd);
	if (fd[0])
		close(fd[0]);
	if (fd[1])
		close(fd[1]);
}

int  verify_epoll_ctl(unsigned int n)
{
	TST_EXP_FAIL(epoll_ctl(*tc[n].epfd, tc[n].opt, *tc[n].fd, tc[n].event),
		     tc[n].exp_err, "epoll_clt(...) if %s", tc[n].desc);
}

void main(){
	setup();
	cleanup();
}
