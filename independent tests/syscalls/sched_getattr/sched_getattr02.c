#include "incl.h"
#define _GNU_SOURCE
char *TCID = "sched_getattr02";

pid_t pid;

pid_t unused_pid;
struct sched_attr attr_copy;

struct test_case {
	pid_t *pid;
	struct sched_attr *a;
	unsigned int size;
	unsigned int flags;
	int exp_errno;
} test_cases[] = {
	{&unused_pid, &attr_copy, sizeof(struct sched_attr), 0, ESRCH},
	{&pid, NULL, sizeof(struct sched_attr), 0, EINVAL},
	{&pid, &attr_copy, sizeof(struct sched_attr) - 1, 0, EINVAL},
	{&pid, &attr_copy, sizeof(struct sched_attr), 1000, EINVAL}
};

void setup(void);

int  sched_getattr_verify(const struct test_case *test);
int TST_TOTAL = ARRAY_SIZE(test_cases);

int  sched_getattr_verify(const struct test_case *test)
{
sched_getattr(*(test->pid), test->a, test->siz;
			test->flags));
	if (TEST_RETURN != -1) {
		tst_resm(TFAIL, "sched_getattr() succeeded unexpectedly.");
		return;
	}
	if (TEST_ERRNO == test->exp_errno) {
		tst_resm(TPASS | TTERRNO,
			"sched_getattr() failed expectedly");
		return;
	}
	tst_resm(TFAIL | TTERRNO, "sched_getattr() failed unexpectedly "
		": expected: %d - %s",
		test->exp_errno, tst_strerrno(test->exp_errno));
}
int main(int argc, char **argv)
{
	int lc, i;
	tst_parse_opts(argc, argv, NULL, NULL);
	setup();
	for (lc = 0; TEST_LOOPING(lc); lc++) {
		for (i = 0; i < TST_TOTAL; i++)
int 			sched_getattr_verify(&test_cases[i]);
	}
	tst_exit();
}
void setup(void)
{
	unused_pid = tst_get_unused_pid(setup);
	if ((tst_kvercmp(3, 14, 0)) < 0)
		tst_brkm(TCONF, NULL, "EDF needs kernel 3.14 or higher");
	TEST_PAUSE;
}

