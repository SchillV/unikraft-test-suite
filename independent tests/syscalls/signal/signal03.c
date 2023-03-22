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

volatile int ign_handler;

void sighandler(int sig LTP_ATTRIBUTE_UNUSED)
{
	ign_handler = 1;
}

void do_test(unsigned int n)
{
	pid_t pid;
	signal(siglist[n], sighandler);
	signal(siglist[n], SIG_IGN);
	pid = getpid();
	kill(pid, siglist[n]);
	TST_EXP_EQ_SSZ(ign_handler, 0);
}

void main(){
	setup();
	cleanup();
}
