#include "incl.h"

void run(void)
{
	struct sched_variant *tv = &sched_variants[tst_variant];
	struct sched_param p = { .sched_priority = 0 };
	TST_EXP_PASS(tv->sched_setparam(0, &p), "sched_setparam(0, 0)");
}

void setup(void)
{
	tst_res(TINFO, "Testing %s variant", sched_variants[tst_variant].desc);
}

void main(){
	setup();
	cleanup();
}
