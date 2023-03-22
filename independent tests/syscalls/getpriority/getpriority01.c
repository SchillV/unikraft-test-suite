#include "incl.h"

struct tcase {
	int which;
	int min;
	int max;
} tcases[] = {
	{PRIO_PROCESS, 0, 0},
	{PRIO_PGRP, 0, 0},
	{PRIO_USER, -20, 0}
};

int  verify_getpriority(unsigned int n)
{
	struct tcase *tc = &tcases[n];
getpriority(tc->which, 0);
	if (TST_ERR != 0) {
		tst_res(TFAIL | TTERRNO, "getpriority(%d, 0) failed",
			tc->which);
		return;
	}
	if (TST_RET < tc->min || TST_RET > tc->max) {
		tst_res(TFAIL, "getpriority(%d, 0) returned %ld, "
			"expected in the range of [%d, %d]",
			tc->which, TST_RET, tc->min, tc->max);
		return;
	}
	tst_res(TPASS, "getpriority(%d, 0) returned %ld",
		tc->which, TST_RET);
}

void main(){
	setup();
	cleanup();
}
