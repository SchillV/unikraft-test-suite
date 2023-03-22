#include "incl.h"

struct test_case_t {
	int size;
	int exp_err;
} tc[] = {
	{0, EINVAL},
	{-1, EINVAL}
};

void run(unsigned int n)
{
	TST_EXP_FAIL(do_epoll_create(tc[n].size),
		     tc[n].exp_err, "epoll_create(%d)", tc[n].size);
}

void main(){
	setup();
	cleanup();
}
