#include "incl.h"
#define INVAL_FLAG	-1
#define INVAL_ID	-1

struct tcase {
	int which;
	int who;
	int exp_errno;
} tcases[] = {
	{INVAL_FLAG, 0, EINVAL},
	{PRIO_PROCESS, INVAL_ID, ESRCH},
	{PRIO_PGRP, INVAL_ID, ESRCH},
	{PRIO_USER, INVAL_ID, ESRCH}
};

int  verify_getpriority(unsigned int n)
{
	struct tcase *tc = &tcases[n];
getpriority(tc->which, tc->who);
	if (TST_RET != -1) {
		tst_res(TFAIL, "getpriority(%d, %d) succeeds unexpectedly, "
			       "returned %li", tc->which, tc->who, TST_RET);
		return;
	}
	if (tc->exp_errno != TST_ERR) {
		tst_res(TFAIL | TTERRNO,
			"getpriority(%d, %d) should fail with %s",
			tc->which, tc->who, tst_strerrno(tc->exp_errno));
		return;
	}
	tst_res(TPASS | TTERRNO, "getpriority(%d, %d) fails as expected",
		tc->which, tc->who);
}

void main(){
	setup();
	cleanup();
}
