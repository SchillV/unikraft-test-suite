#include "incl.h"

pid_t pids[2] = {0, 0};

int  verify_sched_getparam(unsigned int n)
{
	pid_t child_pid;
	struct sched_variant *tv = &sched_variants[tst_variant];
	struct sched_param param = {
		.sched_priority = 100,
	};
	child_pid = fork();
	if (!child_pid) {
		TST_EXP_PASS_SILENT(tv->sched_getparam(pids[n], &param),
				   "sched_getparam(%d)", pids[n]);
		if (!TST_PASS)
			exit(0);
		 * For normal process, scheduling policy is SCHED_OTHER.
		 * For this scheduling policy, only allowed priority value is 0.
		 */
		if (param.sched_priority)
			tst_res(TFAIL,
				"sched_getparam(%d) got wrong sched_priority %d, expected 0",
				pids[n], param.sched_priority);
		else
			tst_res(TPASS, "sched_getparam(%d) got expected sched_priority 0", pids[n]);
		exit(0);
	}
	tst_reap_children();
}

void setup(void)
{
	struct sched_variant *tv = &sched_variants[tst_variant];
	tst_res(TINFO, "Testing %s variant", tv->desc);
	pids[1] = getpid();
}

void main(){
	setup();
	cleanup();
}
