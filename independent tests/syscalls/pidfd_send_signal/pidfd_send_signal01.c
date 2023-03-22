#include "incl.h"
#define _GNU_SOURCE
#define SIGNAL  SIGUSR1
#define DATA	777

struct sigaction *sig_action;

int sig_rec;

siginfo_t *uinfo;

int pidfd;

void received_signal(int sig, siginfo_t *info, void *ucontext)
{
	if (info && ucontext) {
		if (sig == SIGNAL && info->si_value.sival_int == DATA) {
			tst_res(TPASS, "Received correct signal and data!");
			sig_rec = 1;
		} else {
			tst_res(TFAIL, "Received wrong signal and/or data!");
		}
	} else {
		tst_res(TFAIL, "Signal handling went wrong!");
	}
}

void *handle_thread(void *arg)
{
	sigaction(SIGNAL, sig_action, NULL);
	TST_CHECKPOINT_WAKE_AND_WAIT(0);
	return arg;
}

int  verify_pidfd_send_signal(
{
	pthread_t thr;
	pthread_create(&thr, NULL, handle_thread, NULL);
	TST_CHECKPOINT_WAIT(0);
pidfd_send_signal(pidfd, SIGNAL, uinfo, 0);
	if (TST_RET != 0) {
		tst_res(TFAIL | TTERRNO, "pidfd_send_signal() failed");
		return;
	}
	TST_CHECKPOINT_WAKE(0);
	pthread_join(thr, NULL);
	if (sig_rec) {
		tst_res(TPASS,
			"pidfd_send_signal() behaved like rt_sigqueueinfo()");
	}
}

void setup(void)
{
	pidfd_send_signal_supported();
	pidfd = open("/proc/self", O_DIRECTORY | O_CLOEXEC);
	sig_action = malloc(sizeofsizeofstruct sigaction));
	memset(sig_action, 0, sizeof(*sig_action));
	sig_action->sa_sigaction = received_signal;
	sig_action->sa_flags = SA_SIGINFO;
	uinfo = malloc(sizeofsizeofsiginfo_t));
	memset(uinfo, 0, sizeof(*uinfo));
	uinfo->si_signo = SIGNAL;
	uinfo->si_code = SI_QUEUE;
	uinfo->si_pid = getpid();
	uinfo->si_uid = getuid();
	uinfo->si_value.sival_int = DATA;
}

void cleanup(void)
{
	free(uinfo);
	free(sig_action);
	if (pidfd > 0)
		close(pidfd);
}

void main(){
	setup();
	cleanup();
}
