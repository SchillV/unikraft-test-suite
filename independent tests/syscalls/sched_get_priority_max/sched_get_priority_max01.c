#include "incl.h"
#define _GNU_SOURCE
#define POLICY_DESC(x) .desc = #x, .policy = x

struct test_case {
	char *desc;
	int policy;
	int retval;
} tcases[] = {
	{POLICY_DESC(SCHED_BATCH), 0},
	{POLICY_DESC(SCHED_DEADLINE), 0},
	{POLICY_DESC(SCHED_FIFO), 99},
	{POLICY_DESC(SCHED_IDLE), 0},
	{POLICY_DESC(SCHED_OTHER), 0},
	{POLICY_DESC(SCHED_RR), 99},
};

void run_test(unsigned int nr)
{
	struct test_case *tc = &tcases[nr];
	TST_EXP_VAL(tst_syscall(__NR_sched_get_priority_max, tc->policy),
			tc->retval, "%s", tc->desc);
}

void main(){
	setup();
	cleanup();
}
