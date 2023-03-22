#include "incl.h"
#define SCHED_INVALID	99

struct sched_param p;

struct sched_param p1 = { .sched_priority = 1 };

pid_t unused_pid;

pid_t init_pid = 1;

pid_t zero_pid;
struct test_cases_t {
	pid_t *pid;
	int policy;
	struct sched_param *p;
	int error;
} tcases[] = {
	{&unused_pid, SCHED_OTHER, &p, ESRCH},
	{&init_pid, SCHED_INVALID, &p, EINVAL},
	{&init_pid, SCHED_OTHER, (struct sched_param *)-1, EFAULT},
	{&zero_pid, SCHED_OTHER, &p1, EINVAL}
};

void setup(void)
{
	tst_res(TINFO, "Testing %s variant", sched_variants[tst_variant].desc);
	unused_pid = tst_get_unused_pid();
}

void run(unsigned int n)
{
	struct sched_variant *tv = &sched_variants[tst_variant];
	struct test_cases_t *tc = &tcases[n];
	TST_EXP_FAIL(tv->sched_setscheduler(*tc->pid, tc->policy, tc->p),
		     tc->error, "sched_setscheduler(%d, %d, %p)",
		     *tc->pid, tc->policy, tc->p);
}

void main(){
	setup();
	cleanup();
}
