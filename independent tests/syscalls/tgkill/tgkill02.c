#include "incl.h"

void *thread_func(void *arg)
{
	const struct rlimit sigpending = {
		.rlim_cur = 0,
		.rlim_max = 0,
	};
	sigset_t sigrtmin;
	int err;
	pid_t *tid = arg;
	sigemptyset(&sigrtmin);
	sigaddset(&sigrtmin, SIGRTMIN);
	err = pthread_sigmask(SIG_BLOCK, &sigrtmin, NULL);
	if (err)
		tst_brk(TBROK, "pthread_sigmask() failed: %s",
			tst_strerrno(err));
	setrlimit(RLIMIT_SIGPENDING, &sigpending);
	TST_CHECKPOINT_WAKE_AND_WAIT(0);
	return arg;
}

void run(void)
{
	pthread_t thread;
	pid_t tid = -1;
	pthread_create(&thread, NULL, thread_func, &tid);
	TST_CHECKPOINT_WAIT(0);
sys_tgkill(getpid(), tid, SIGRTMIN);
	if (TST_RET && TST_ERR == EAGAIN)
		tst_res(TPASS, "tgkill() failed with EAGAIN as expected");
	else
		tst_res(TFAIL | TTERRNO,
			"tgkill() should have failed with EAGAIN");
	TST_CHECKPOINT_WAKE(0);
	pthread_join(thread, NULL);
}

void main(){
	setup();
	cleanup();
}
