#include "incl.h"
#define SCHED_INVALID 1000

void verif_sched_get_priority_max02(void)
{
	TST_EXP_FAIL(tst_syscall(__NR_sched_get_priority_max, SCHED_INVALID), EINVAL);
}

void main(){
	setup();
	cleanup();
}
