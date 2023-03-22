#include "incl.h"

void do_child(void)
{
	abort();
	tst_res(TFAIL, "Abort returned");
	exit(0);
}
int  verify_abort(
{
	int status, kidpid;
	int sig, core;
	kidpid = fork();
	if (kidpid == 0)
		do_child();
	wait(&status);
	if (!WIFSIGNALED(status)) {
		tst_res(TFAIL, "Child %s, expected SIGIOT",
			tst_strstatus(status));
		return;
	}
	core = WCOREDUMP(status);
	sig = WTERMSIG(status);
	if (core == 0)
		tst_res(TFAIL, "abort() failed to dump core");
	else
		tst_res(TPASS, "abort() dumped core");
	if (sig == SIGIOT)
		tst_res(TPASS, "abort() raised SIGIOT");
	else
		tst_res(TFAIL, "abort() raised %s", tst_strsig(sig));
}
#define MIN_RLIMIT_CORE (512 * 1024)

void setup(void)
{
	struct rlimit rlim;
	getrlimit(RLIMIT_CORE, &rlim);
	if (rlim.rlim_max < MIN_RLIMIT_CORE) {
		if (geteuid() != 0) {
			tst_brk(TCONF, "hard limit(%lu)less than MIN_RLIMT_CORE(%i)",
				rlim.rlim_max, MIN_RLIMIT_CORE);
		}
		tst_res(TINFO, "Raising rlim_max to %i", MIN_RLIMIT_CORE);
		rlim.rlim_max = MIN_RLIMIT_CORE;
	}
	if (rlim.rlim_cur < MIN_RLIMIT_CORE) {
		rlim.rlim_cur = MIN_RLIMIT_CORE;
		setrlimit(RLIMIT_CORE, &rlim);
	}
}

void main(){
	setup();
	cleanup();
}
