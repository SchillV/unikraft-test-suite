#include "incl.h"

void run(void)
{
	struct sched_variant *tv = &sched_variants[tst_variant];
	pid_t child_pid = fork();
	if (!child_pid) {
		struct sched_param p = { .sched_priority = 0 };
		struct passwd *pw = getpwnam("nobody");
		seteuid(pw->pw_uid);
		TST_EXP_FAIL(tv->sched_setparam(getppid(), &p), EPERM,
			     "sched_setparam(%d, 0)", getppid());
		exit(0);
	}
	tst_reap_children();
}

void setup(void)
{
	tst_res(TINFO, "Testing %s variant", sched_variants[tst_variant].desc);
}

void main(){
	setup();
	cleanup();
}
