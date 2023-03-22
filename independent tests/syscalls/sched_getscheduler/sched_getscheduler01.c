#include "incl.h"
struct test_cases_t {
	int priority;
	int policy;
} tcases[] = {
	{1, SCHED_RR},
	{0, SCHED_OTHER},
	{1, SCHED_FIFO}
};

void run(unsigned int n)
{
	struct sched_variant *tv = &sched_variants[tst_variant];
	struct test_cases_t *tc = &tcases[n];
	struct sched_param p = { .sched_priority = tc->priority };
	TST_EXP_PASS_SILENT(tv->sched_setscheduler(0, tc->policy, &p));
	if (!TST_PASS)
		return;
tv->sched_getscheduler(0);
	if (TST_RET == tc->policy)
		tst_res(TPASS, "got expected policy %d", tc->policy);
	else
		tst_res(TFAIL | TERRNO, "got policy %ld, expected %d", TST_RET, tc->policy);
}

void setup(void)
{
	tst_res(TINFO, "Testing %s variant", sched_variants[tst_variant].desc);
}

void main(){
	setup();
	cleanup();
}
