#include "incl.h"

void run(void)
{
	struct sched_variant *tv = &sched_variants[tst_variant];
	struct sched_param p5 = { .sched_priority = 5 };
	struct sched_param p;
	pid_t child_pid;
	child_pid = fork();
	if (!child_pid) {
		TST_EXP_PASS_SILENT(tv->sched_setparam(getppid(), &p5));
		exit(0);
	}
	tst_reap_children();
	TST_EXP_PASS_SILENT(tv->sched_getparam(0, &p));
	if (p.sched_priority == p5.sched_priority)
		tst_res(TPASS, "got expected priority %d", p.sched_priority);
	else
		tst_res(TFAIL, "got priority %d, expected 5", p.sched_priority);
}
void setup(void)
{
	struct sched_variant *tv = &sched_variants[tst_variant];
	struct sched_param p = { .sched_priority = 1 };
	tst_res(TINFO, "Testing %s variant", tv->desc);
	if (tv->sched_setscheduler(0, SCHED_FIFO, &p))
		tst_brk(TBROK | TERRNO, "sched_setcheduler(0, SCHED_FIFO, 1)");
}

void main(){
	setup();
	cleanup();
}
