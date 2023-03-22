#include "incl.h"

int child_fn(void *arg LTP_ATTRIBUTE_UNUSED);

struct tcase {
	int (*child_fn)(void *arg);
	void *child_stack;
	int exp_errno;
	char err_desc[10];
} tcases[] = {
	{child_fn, NULL, EINVAL, "NULL stack"},
};

int child_fn(void *arg LTP_ATTRIBUTE_UNUSED)
{
	exit(0);
}

int  verify_clone(unsigned int nr)
{
	struct tcase *tc = &tcases[nr];
	TST_EXP_FAIL(ltp_clone(0, tc->child_fn, NULL,
				CHILD_STACK_SIZE, tc->child_stack),
				tc->exp_errno, "%s", tc->err_desc);
}

void main(){
	setup();
	cleanup();
}
