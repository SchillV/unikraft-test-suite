#include "incl.h"
#ifdef HAVE_STRUCT_SIGACTION_SA_SIGACTION
#define SIGNAL	SIGUSR1
#define DATA	777

struct sigaction *sig_action;

int sig_rec;

siginfo_t *uinfo;

pid_t tid;

void received_signal(int sig, siginfo_t *info, void *ucontext)
{
	if (info && ucontext) {
		if (sig == SIGNAL && info->si_value.sival_int == DATA) {
			tst_res(TPASS, "Received correct signal and data!");
			sig_rec = 1;
		} else
			tst_res(TFAIL, "Received wrong signal and/or data!");
	} else
		tst_res(TFAIL, "Signal handling went wrong!");
}

void *handle_thread(void *arg LTP_ATTRIBUTE_UNUSED)
{
	int ret;
	tid = tst_syscall(__NR_gettid);
	ret = sigaction(SIGNAL, sig_action, NULL);
	if (ret)
		tst_brk(TBROK, "Failed to set sigaction for handler thread!");
	TST_CHECKPOINT_WAKE(0);
	TST_CHECKPOINT_WAIT(1);
	return arg;
}

int  verify_sigqueueinfo(
{
	pthread_t thr;
	pthread_create(&thr, NULL, handle_thread, NULL);
	TST_CHECKPOINT_WAIT(0);
sys_rt_sigqueueinfo(tid, SIGNAL, uinfo);
	if (TST_RET != 0) {
		tst_res(TFAIL | TTERRNO, "rt_sigqueueinfo() failed");
		return;
	}
	TST_CHECKPOINT_WAKE(1);
	pthread_join(thr, NULL);
	if (sig_rec)
		tst_res(TPASS, "rt_sigqueueinfo() was successful!");
}

void setup(void)
{
	sig_action = malloc(sizeofsizeofstruct sigaction));
	memset(sig_action, 0, sizeof(*sig_action));
	sig_action->sa_sigaction = received_signal;
	sig_action->sa_flags = SA_SIGINFO;
	uinfo = malloc(sizeofsizeofsiginfo_t));
	memset(uinfo, 0, sizeof(*uinfo));
	uinfo->si_code = SI_QUEUE;
	uinfo->si_pid = getpid();
	uinfo->si_uid = getuid();
	uinfo->si_value.sival_int = DATA;
	sig_rec = 0;
}

void cleanup(void)
{
	free(uinfo);
	free(sig_action);
}

