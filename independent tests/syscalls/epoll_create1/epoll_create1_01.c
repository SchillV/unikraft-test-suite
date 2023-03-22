#include "incl.h"

struct test_case_t {
	int flags;
	int exp_flag;
	const char *desc;
} tc[] = {
	{0, 0, "without EPOLL_CLOEXEC"},
	{EPOLL_CLOEXEC, 1, "with EPOLL_CLOEXEC"}
};

void run(unsigned int n)
{
	int fd, coe;
	fd = tst_syscall(__NR_epoll_create1, tc[n].flags);
	if (fd == -1)
		tst_brk(TFAIL | TERRNO, "epoll_create1(...) failed");
	coe = fcntl(fd, F_GETFD);
	if ((coe & FD_CLOEXEC) != tc[n].exp_flag)
		tst_res(TFAIL, "epoll_create1(...) %s", tc[n].desc);
	else
		tst_res(TPASS, "epoll_create1(...) %s", tc[n].desc);
	close(fd);
}

void main(){
	setup();
	cleanup();
}
