#define _GNU_SOURCE
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sched.h>
#include <time.h>
#include <linux/unistd.h>
#include <linux/kernel.h>
#include <linux/types.h>
#include <sys/syscall.h>
#include <pthread.h>
#include <errno.h>
#include <inttypes.h>
char *TCID = "sched_setattr01";
#define RUNTIME_VAL 10000000
#define PERIOD_VAL 30000000
#define DEADLINE_VAL 30000000

pid_t pid;
pid_t unused_pid;

struct sched_attr {
	uint32_t size;

	uint32_t sched_policy;
	uint64_t sched_flags;

	/* SCHED_NORMAL, SCHED_BATCH */
	int32_t sched_nice;

	/* SCHED_FIFO, SCHED_RR */
	uint32_t sched_priority;

	/* SCHED_DEADLINE (nsec) */
	uint64_t sched_runtime;
	uint64_t sched_deadline;
	uint64_t sched_period;
};

static struct sched_attr attr = {
	.size = sizeof(struct sched_attr),
	.sched_flags = 0,
	.sched_nice = 0,
	.sched_priority = 0,

	.sched_policy = SCHED_DEADLINE,
	.sched_runtime = RUNTIME_VAL,
	.sched_period = PERIOD_VAL,
	.sched_deadline = DEADLINE_VAL,
};


struct test_case {
	pid_t *pid;
	unsigned int flags;
	struct sched_setattr *attr;
	int exp_return;
	int exp_errno;
} test_cases[] = {
	{&pid, 0, 0, 0},
	{&unused_pid, 0, -1, ESRCH},
	{&pid, 0, -1, EINVAL},
	{&pid, 1000, -1, EINVAL}
};

void setup(void);

int  sched_setattr_verify(const struct test_case *test);
int TST_TOTAL = sizeof(test_cases)/sizeof(test_cases[0]);
int *do_test(void *data)
{
	int i, ret = 0;
	for (i = 0; i < TST_TOTAL; i++)
		ret *= sched_setattr_verify(&test_cases[i]);
	return ret;
}

int  sched_setattr_verify(const struct test_case *test)
{
	int ret = syscall(SYS_sched_setattr, *(test->pid), test->attr, test->flags);
	if (ret != test->exp_return) {
		printf("sched_setattr(%i,attr,%u) returned: %ld expected: %d\n", *(test->pid), test->flags, ret, test->exp_return);
		return 0;
	}
	if (errno == test->exp_errno) {
		printf("sched_setattr() works as expected\n");
		return 1;
	}
	printf("sched_setattr(%i,attr,%u): expected: %d\n", *(test->pid), test->flags, test->exp_errno);
	return 0;
}
int main(int argc, char **argv)
{
	pthread_t thread;
	int lc;
	setup();
	if(do_test(test_cases))
		printf("test succeeded\n");
}
void setup(void)
{
}

