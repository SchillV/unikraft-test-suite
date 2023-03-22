#include "incl.h"

volatile sig_atomic_t sig_flag;

void sighandler(int sig)
{
	if (sig == SIGUSR1)
		sig_flag = 1;
}

void setup(void)
{
	signal(SIGUSR1, sighandler);
}

void run(void)
{
	int tid;
	int timeout_ms = 1000;
	sig_flag = 0;
	tid = tst_syscall(__NR_gettid);
	TST_EXP_PASS(tst_syscall(__NR_tkill, tid, SIGUSR1));
	while (timeout_ms--) {
		if (sig_flag)
			break;
		usleep(1000);
	}
	if (sig_flag)
		tst_res(TPASS, "signal captured");
	else
		tst_res(TFAIL, "signal not captured");
}

void main(){
	setup();
	cleanup();
}
