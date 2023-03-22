#include "incl.h"

int siglist[] = { SIGHUP, SIGINT, SIGQUIT, SIGILL, SIGTRAP, SIGABRT,
	SIGBUS, SIGFPE, SIGUSR1, SIGSEGV, SIGUSR2, SIGPIPE, SIGALRM,
	SIGTERM, SIGCHLD, SIGCONT, SIGTSTP, SIGTTIN,
	SIGTTOU, SIGURG, SIGXCPU, SIGXFSZ, SIGVTALRM, SIGPROF,
	SIGWINCH, SIGIO, SIGPWR, SIGSYS
};

void sighandler(int sig LTP_ATTRIBUTE_UNUSED)
{
}

void do_test(unsigned int n)
{
	sighandler_t rval, first;
	signal(siglist[n], SIG_DFL);
	first = signal(siglist[n], sighandler);
	signal(siglist[n], SIG_DFL);
	rval = signal(siglist[n], sighandler);
	if (rval == first) {
		tst_res(TPASS, "signal(%d) restore succeeded "
				"received %p.", siglist[n], rval);
	} else {
		tst_res(TFAIL, "return values for signal(%d) don't match. "
				"Got %p, expected %p.",
				siglist[n], rval, first);
	}
}

void main(){
	setup();
	cleanup();
}
