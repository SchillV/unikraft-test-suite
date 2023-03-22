#include "incl.h"

struct sched_param param;

pid_t unused_pid;

pid_t zero_pid;

pid_t inval_pid = -1;

struct test_case_t {
	char *desc;
	pid_t *pid;
	struct sched_param *p;
	int exp_errno;
} test_cases[] = {
	{"sched_getparam() with non-existing pid",
	 &unused_pid, &param, ESRCH},
	{"sched_getparam() with invalid pid",
	 &inval_pid, &param, EINVAL},
	{"sched_getparam() with invalid address for param",
	 &zero_pid, NULL, EINVAL},
};

int  verify_sched_getparam(unsigned int n)
{
	struct test_case_t *tc = &test_cases[n];
	struct sched_variant *tv = &sched_variants[tst_variant];
	TST_EXP_FAIL(tv->sched_getparam(*(tc->pid), tc->p), tc->exp_errno,
		     "%s", tc->desc);
}

void setup(void)
{
	tst_res(TINFO, "Testing %s variant", sched_variants[tst_variant].desc);
	unused_pid = tst_get_unused_pid();
}

void main(){
	setup();
	cleanup();
}
