#include "incl.h"

struct test_case_t {
	int flags;
	int exp_err;
	const char *desc;
} tc[] = {
	{-1, EINVAL, "-1"},
	{EPOLL_CLOEXEC + 1, EINVAL, "EPOLL_CLOEXEC+1"}
};

void run(unsigned int n)
{
	TST_EXP_FAIL(tst_syscall(__NR_epoll_create1, tc[n].flags),
		     tc[n].exp_err, "epoll_create1(%s)", tc[n].desc);
}

void main(){
	setup();
	cleanup();
}
