#include "incl.h"
#define _GNU_SOURCE

struct tcase {
	int sig;
	int dumps_core;
} tcases[] = {
	{SIGHUP, 0},
	{SIGINT, 0},
	{SIGQUIT, 1},
	{SIGILL, 1},
	{SIGTRAP, 1},
	{SIGABRT, 1},
	{SIGIOT, 1},
	{SIGBUS, 1},
	{SIGFPE, 1},
	{SIGKILL, 0},
	{SIGUSR1, 0},
	{SIGSEGV, 1},
	{SIGUSR2, 0},
	{SIGPIPE, 0},
	{SIGALRM, 0},
	{SIGTERM, 0},
#ifdef SIGSTKFLT
	{SIGSTKFLT, 0},
#endif
	{SIGXCPU, 1},
	{SIGXFSZ, 1},
	{SIGVTALRM, 0},
	{SIGPROF, 0},
	{SIGIO, 0},
	{SIGPWR, 0},
	{SIGSYS, 1},
};

int  verify_kill(unsigned int n)
{
	int core;
	pid_t pid, npid;
	int nsig, status;
	struct tcase *tc = &tcases[n];
	pid = fork();
	if (!pid)
		pause();
	kill(pid, tc->sig);
	npid = wait(&status);
	if (npid != pid) {
		tst_res(TFAIL, "wait() returned %d, expected %d", npid, pid);
		return;
	}
	nsig = WTERMSIG(status);
	core = WCOREDUMP(status);
	if (tc->dumps_core) {
		if (!core) {
			tst_res(TFAIL, "core dump bit not set for %s", tst_strsig(tc->sig));
			return;
		}
	} else {
		if (core) {
			tst_res(TFAIL, "core dump bit set for %s", tst_strsig(tc->sig));
			return;
		}
	}
	if (nsig != tc->sig) {
		tst_res(TFAIL, "wait: unexpected signal %d returned, expected %d", nsig, tc->sig);
		return;
	}
	tst_res(TPASS, "signal %-16s%s", tst_strsig(tc->sig),
			tc->dumps_core ? " dumped core" : "");
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
		tst_res(TINFO, "Adjusting RLIMIT_CORE to %i", MIN_RLIMIT_CORE);
		rlim.rlim_cur = MIN_RLIMIT_CORE;
		setrlimit(RLIMIT_CORE, &rlim);
	}
}

void main(){
	setup();
	cleanup();
}
