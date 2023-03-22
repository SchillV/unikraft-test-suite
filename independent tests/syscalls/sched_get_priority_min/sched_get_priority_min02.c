#include "incl.h"
#define SCHED_INVALID 1000

void verif_sched_get_priority_min02(void)
{
	TST_EXP_FAIL(tst_syscall(__NR_sched_get_priority_min, SCHED_INVALID), EINVAL);
}

void main(){
	setup();
	cleanup();
}
