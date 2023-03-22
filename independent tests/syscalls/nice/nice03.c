#include "incl.h"
#define	NICEINC	2
#define MAX_PRIO 19

void nice_test(void)
{
	int new_nice;
	int orig_nice;
	int exp_nice;
	orig_nice = getpriority(PRIO_PROCESS, 0);
nice(NICEINC);
	if (TST_RET == -1) {
		tst_res(TFAIL | TTERRNO, "nice(%d) returned -1", NICEINC);
		return;
	}
	if (TST_ERR) {
		tst_res(TFAIL | TTERRNO, "nice(%d) failed", NICEINC);
		return;
	}
	new_nice = getpriority(PRIO_PROCESS, 0);
	exp_nice = MIN(MAX_PRIO, (orig_nice + NICEINC));
	if (new_nice != exp_nice) {
		tst_res(TFAIL, "Process priority %i, expected %i",
				new_nice, exp_nice);
		return;
	}
	tst_res(TPASS, "nice(%d) passed", NICEINC);
	exit(0);
}

int  verify_nice(
{
	if (!fork())
		nice_test();
	tst_reap_children();
}

void main(){
	setup();
	cleanup();
}
