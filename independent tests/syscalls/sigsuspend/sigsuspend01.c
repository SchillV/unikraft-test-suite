#include "incl.h"

sigset_t signalset, sigset1, sigset2;

volatile sig_atomic_t alarm_num;

void sig_handler(int sig)
{
	alarm_num = sig;
}

int  verify_sigsuspend(
{
	alarm_num = 0;
	sigfillset(&sigset2);
	alarm(1);
sigsuspend(&signalset);
	alarm(0);
	if (TST_RET != -1 || TST_ERR != EINTR) {
		tst_res(TFAIL | TTERRNO,
			"sigsuspend() returned value %ld", TST_RET);
		return;
	}
	if (alarm_num != SIGALRM) {
		tst_res(TFAIL, "sigsuspend() didn't unblock SIGALRM");
		return;
	}
	sigprocmask(0, NULL, &sigset2);
	if (memcmp(&sigset1, &sigset2, sizeof(unsigned long))) {
		tst_res(TFAIL, "sigsuspend() failed to "
			"restore the previous signal mask");
		return;
	}
	tst_res(TPASS, "sigsuspend() succeeded");
}

void setup(void)
{
	sigemptyset(&signalset);
	sigemptyset(&sigset1);
	sigaddset(&sigset1, SIGALRM);
	struct sigaction sa_new = {
		.sa_handler = sig_handler,
	};
	sigaction(SIGALRM, &sa_new, 0);
	sigprocmask(SIG_SETMASK, &sigset1, NULL);
}

void main(){
	setup();
	cleanup();
}
