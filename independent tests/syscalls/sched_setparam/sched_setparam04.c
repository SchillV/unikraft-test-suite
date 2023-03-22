#include "incl.h"

struct sched_param p = { .sched_priority = 0 };

struct sched_param p1 = { .sched_priority = 1 };

pid_t unused_pid;

pid_t inval_pid = -1;

pid_t zero_pid;

struct test_cases_t {
	char *desc;
	pid_t *pid;
	struct sched_param *p;
	int exp_errno;
} tcases[] = {
	{
	"test with non-existing pid", &unused_pid, &p, ESRCH}, {
	"test invalid pid value", &inval_pid, &p, EINVAL,}, {
	"test with invalid address for p", &zero_pid, NULL, EINVAL}, {
	"test with invalid p.sched_priority", &zero_pid, &p1, EINVAL}
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
	TST_EXP_FAIL(tv->sched_setparam(*tc->pid, tc->p), tc->exp_errno, "%s", tc->desc);
}

void main(){
	setup();
	cleanup();
}
