#include "incl.h"

siginfo_t *infop;

int core_dumps = 1;

void run(void)
{
	pid_t pidchild;
	 * Triggering SIGFPE by invalid instruction is not always possible,
	 * some architectures does not trap division-by-zero at all and even
	 * when it's possible we would have to fight the compiler optimizations
	 * that have tendency to remove undefined operations.
	 */
	pidchild = fork();
	if (!pidchild)
		raise(SIGFPE);
	TST_EXP_PASS(waitid(P_ALL, 0, infop, WEXITED));
	TST_EXP_EQ_LI(infop->si_pid, pidchild);
	TST_EXP_EQ_LI(infop->si_status, SIGFPE);
	TST_EXP_EQ_LI(infop->si_signo, SIGCHLD);
	if (core_dumps)
		TST_EXP_EQ_LI(infop->si_code, CLD_DUMPED);
	else
		TST_EXP_EQ_LI(infop->si_code, CLD_KILLED);
}

void setup(void)
{
	struct rlimit rlim;
	char c;
	getrlimit(RLIMIT_CORE, &rlim);
	file_scanf("/proc/sys/kernel/core_pattern", "%c", &c);
	if (rlim.rlim_cur)
		return;
	if (!rlim.rlim_max) {
		if (c != '|')
			core_dumps = 0;
		return;
	}
	tst_res(TINFO, "Raising RLIMIT_CORE rlim_cur=%li -> %li",
	        rlim.rlim_cur, rlim.rlim_max);
	rlim.rlim_cur = rlim.rlim_max;
	setrlimit(RLIMIT_CORE, &rlim);
}

void main(){
	setup();
	cleanup();
}
