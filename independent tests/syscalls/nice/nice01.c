#include "incl.h"
#define MIN_PRIO        -20

int nice_inc[] = {-1, -12, -50};

int  verify_nice(unsigned int i)
{
	int new_nice;
	int orig_nice;
	int exp_nice;
	int inc = nice_inc[i];
	int delta;
	orig_nice = getpriority(PRIO_PROCESS, 0);
nice(inc);
	exp_nice = MAX(MIN_PRIO, (orig_nice + inc));
	if (TST_RET != exp_nice) {
		tst_res(TFAIL | TTERRNO, "nice(%d) returned %li, expected %i",
				inc, TST_RET, exp_nice);
		return;
	}
	if (TST_ERR) {
		tst_res(TFAIL | TTERRNO, "nice(%d) failed", inc);
		return;
	}
	new_nice = getpriority(PRIO_PROCESS, 0);
	if (new_nice != exp_nice) {
		tst_res(TFAIL, "Process priority %i, expected %i",
				new_nice, exp_nice);
		return;
	}
	tst_res(TPASS, "nice(%d) passed", inc);
	delta = orig_nice - exp_nice;
nice(delta);
	if (TST_ERR)
		tst_brk(TBROK | TTERRNO, "nice(%d) failed", delta);
}

void main(){
	setup();
	cleanup();
}
