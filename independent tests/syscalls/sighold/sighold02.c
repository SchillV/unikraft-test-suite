#include "incl.h"
#define _XOPEN_SOURCE 600
#ifndef NSIG
#	define NSIG _NSIG
#endif
#ifndef NUMSIGS
#	define NUMSIGS NSIG
#endif

int sigs_catched;

int sigs_map[NUMSIGS];

int skip_sig(int sig)
{
	if (sig >= 32 && sig < SIGRTMIN)
		return 1;
	switch (sig) {
	case SIGCHLD:
	case SIGKILL:
	case SIGALRM:
	case SIGSTOP:
		return 1;
	default:
		return 0;
	}
}

void handle_sigs(int sig)
{
	sigs_map[sig] = 1;
	sigs_catched++;
}

void do_child(void)
{
	int sig;
	for (sig = 1; sig < NUMSIGS; sig++) {
		if (skip_sig(sig))
			continue;
		signal(sig, handle_sigs);
	}
	for (sig = 1; sig < NUMSIGS; sig++) {
		if (skip_sig(sig))
			continue;
		if (sighold(sig))
			tst_brk(TBROK | TERRNO, "sighold(%s %i)", tst_strsig(sig), sig);
	}
	TST_CHECKPOINT_WAKE_AND_WAIT(0);
	if (!sigs_catched) {
		tst_res(TPASS, "all signals were hold");
		return;
	}
	tst_res(TFAIL, "signal handler was executed");
	for (sig = 1; sig < NUMSIGS; sig++)
		if (sigs_map[sig])
			tst_res(TINFO, "Signal %i(%s) catched", sig, tst_strsig(sig));
}

void run(void)
{
	pid_t pid_child;
	int signal;
	pid_child = fork();
	if (!pid_child) {
		do_child();
		return;
	}
	TST_CHECKPOINT_WAIT(0);
	for (signal = 1; signal < NUMSIGS; signal++) {
		if (skip_sig(signal))
			continue;
		kill(pid_child, signal);
	}
	TST_CHECKPOINT_WAKE(0);
}

void main(){
	setup();
	cleanup();
}
