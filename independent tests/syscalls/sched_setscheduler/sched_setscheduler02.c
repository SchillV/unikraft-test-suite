#include "incl.h"

uid_t nobody_uid;

void setup(void)
{
	struct passwd *pw = getpwnam("nobody");
	tst_res(TINFO, "Testing %s variant", sched_variants[tst_variant].desc);
	nobody_uid = pw->pw_uid;
}

void run(void)
{
	struct sched_variant *tv = &sched_variants[tst_variant];
	pid_t pid = fork();
	if (!pid) {
		struct sched_param p = { .sched_priority = 1 };
		seteuid(nobody_uid);
		TST_EXP_FAIL(tv->sched_setscheduler(0, SCHED_FIFO, &p), EPERM,
			     "sched_setscheduler(0, SCHED_FIFO, %d)",
			     p.sched_priority);
		exit(0);
	}
	tst_reap_children();
}

void main(){
	setup();
	cleanup();
}
