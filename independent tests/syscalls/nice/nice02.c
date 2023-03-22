#include "incl.h"
#define	NICEINC 50
#define MAX_PRIO 19
#define DEFAULT_PRIO 0

int  verify_nice(
{
	int new_nice;
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
	if (new_nice != MAX_PRIO) {
		tst_res(TFAIL, "Process priority %i, expected %i",
			new_nice, MAX_PRIO);
		return;
	}
	tst_res(TPASS, "nice(%d) passed", NICEINC);
nice(DEFAULT_PRIO);
	if (TST_ERR)
		tst_brk(TBROK | TTERRNO, "nice(%d) failed", DEFAULT_PRIO);
}

void main(){
	setup();
	cleanup();
}
