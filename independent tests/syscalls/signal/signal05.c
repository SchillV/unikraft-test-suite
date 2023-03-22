#include "incl.h"

int siglist[] = { SIGHUP, SIGINT, SIGQUIT, SIGILL, SIGTRAP, SIGABRT, SIGIOT,
	SIGBUS, SIGFPE, SIGUSR1, SIGSEGV, SIGUSR2, SIGPIPE, SIGALRM,
	SIGTERM,
#ifdef SIGSTKFLT
	SIGSTKFLT,
#endif
	SIGCHLD, SIGCONT, SIGTSTP, SIGTTIN,
	SIGTTOU, SIGURG, SIGXCPU, SIGXFSZ, SIGVTALRM, SIGPROF,
	SIGWINCH, SIGIO, SIGPWR, SIGSYS,
#ifdef SIGUNUSED
	SIGUNUSED
#endif
};

volatile int sig_pass;

void sighandler(int sig)
{
	sig_pass = sig;
}

void do_test(unsigned int n)
{
	pid_t pid;
	signal(siglist[n], sighandler);
	pid = getpid();
	kill(pid, siglist[n]);
	TST_EXP_EQ_SSZ(siglist[n], sig_pass);
}

void main(){
	setup();
	cleanup();
}
