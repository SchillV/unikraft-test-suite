#include "incl.h"

pid_t unused_pid;

void setup(void)
{
	tst_res(TINFO, "Testing %s variant", sched_variants[tst_variant].desc);
	unused_pid = tst_get_unused_pid();
}

void run(void)
{
	struct sched_variant *tv = &sched_variants[tst_variant];
	TST_EXP_FAIL(tv->sched_getscheduler(unused_pid), ESRCH,
		     "sched_getscheduler(%d)", unused_pid);
}

void main(){
	setup();
	cleanup();
}
