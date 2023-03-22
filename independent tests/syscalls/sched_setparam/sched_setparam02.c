#include "incl.h"
#define FIFO_OR_RR_PRIO 5
#define OTHER_PRIO 0

struct test_cases_t {
	char *desc;
	int policy;
	int priority;
	int param;
} tcases[] = {
	{
	"Test with policy SCHED_FIFO", SCHED_FIFO, FIFO_OR_RR_PRIO, 1}, {
	"Test with policy SCHED_RR", SCHED_RR, FIFO_OR_RR_PRIO, 1}, {
	"Test with SCHED_OTHER", SCHED_OTHER, OTHER_PRIO, 0}
};

void run(unsigned int n)
{
	struct test_cases_t *tc = &tcases[n];
	struct sched_variant *tv = &sched_variants[tst_variant];
	struct sched_param p = { .sched_priority = tc->param };
	TST_EXP_PASS_SILENT(tv->sched_setscheduler(0, tc->policy, &p));
	p.sched_priority = tc->priority;
	TST_EXP_PASS_SILENT(tv->sched_setparam(0, &p));
	p.sched_priority = -1;
	TST_EXP_PASS_SILENT(tv->sched_getparam(0, &p));
	if (p.sched_priority == tc->priority) {
		tst_res(TPASS, "got expected priority %d", p.sched_priority);
	} else {
		tst_res(TFAIL, "unexpected priority value %d, expected %d",
			p.sched_priority, tc->priority);
	}
}

void setup(void)
{
	tst_res(TINFO, "Testing %s variant", sched_variants[tst_variant].desc);
}

void main(){
	setup();
	cleanup();
}
