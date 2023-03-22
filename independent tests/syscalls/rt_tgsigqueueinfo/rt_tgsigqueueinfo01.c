#include "incl.h"
#define _GNU_SOURCE
#ifndef __ANDROID__
#define SI_SIGVAL si_sigval
#else
#define SI_SIGVAL _sigval
#endif

char sigval_send[] = "rt_tgsigqueueinfo data";

volatile int signum_rcv;

char *sigval_rcv;

void sigusr1_handler(int signum, siginfo_t *uinfo,
			    void *p LTP_ATTRIBUTE_UNUSED)
{
	signum_rcv = signum;
	sigval_rcv = uinfo->_sifields._rt.SI_SIGVAL.sival_ptr;
}
void *send_rcv_func(void *arg)
{
	siginfo_t uinfo;
	signum_rcv = 0;
	sigval_rcv = NULL;
	uinfo.si_errno = 0;
	uinfo.si_code = SI_QUEUE;
	uinfo._sifields._rt.SI_SIGVAL.sival_ptr = sigval_send;
tst_syscall(__NR_rt_tgsigqueueinfo, getpid(;
			 syscall(__NR_gettid), SIGUSR1, &uinfo));
	if (TST_RET)
		tst_brk(TFAIL | TTERRNO, "rt_tgsigqueueinfo failed");
	while (!signum_rcv)
		usleep(1000);
	if ((signum_rcv == SIGUSR1) && (sigval_rcv == sigval_send))
		tst_res(TPASS, "Test signal to self succeeded");
	else
		tst_res(TFAIL, "Failed to deliver signal/data to self thread");
	return arg;
}

int  verify_signal_self(
{
	pthread_t pt;
	pthread_create(&pt, NULL, send_rcv_func, NULL);
	pthread_join(pt, NULL);
}
void *receiver_func(void *arg)
{
	pid_t *tid = arg;
	signum_rcv = 0;
	sigval_rcv = NULL;
	TST_CHECKPOINT_WAKE(0);
	while (!signum_rcv)
		usleep(1000);
	if ((signum_rcv == SIGUSR1) && (sigval_rcv == sigval_send))
		tst_res(TPASS, "Test signal to different thread succeeded");
	else
		tst_res(TFAIL,
			"Failed to deliver signal/data to different thread");
	return NULL;
}

int  verify_signal_parent_thread(
{
	pid_t tid = -1;
	pthread_t pt;
	siginfo_t uinfo;
	pthread_create(&pt, NULL, receiver_func, &tid);
	TST_CHECKPOINT_WAIT(0);
	uinfo.si_errno = 0;
	uinfo.si_code = SI_QUEUE;
	uinfo._sifields._rt.SI_SIGVAL.sival_ptr = sigval_send;
tst_syscall(__NR_rt_tgsigqueueinfo, getpid(;
			 tid, SIGUSR1, &uinfo));
	if (TST_RET)
		tst_brk(TFAIL | TTERRNO, "rt_tgsigqueueinfo failed");
	pthread_join(pt, NULL);
}
void *sender_func(void *arg)
{
	pid_t *tid = arg;
	siginfo_t uinfo;
	uinfo.si_errno = 0;
	uinfo.si_code = SI_QUEUE;
	uinfo._sifields._rt.SI_SIGVAL.sival_ptr = sigval_send;
tst_syscall(__NR_rt_tgsigqueueinfo, getpid(;
			 *tid, SIGUSR1, &uinfo));
	if (TST_RET)
		tst_brk(TFAIL | TTERRNO, "rt_tgsigqueueinfo failed");
	return NULL;
}

int  verify_signal_inter_thread(
{
	pid_t tid = -1;
	pthread_t pt1, pt2;
	pthread_create(&pt1, NULL, receiver_func, &tid);
	TST_CHECKPOINT_WAIT(0);
	pthread_create(&pt2, NULL, sender_func, &tid);
	pthread_join(pt2, NULL);
	pthread_join(pt1, NULL);
}

struct tcase {
	void (*tfunc)(void);
} tcases[] = {
int 	{&verify_signal_self},
int 	{&verify_signal_parent_thread},
int 	{&verify_signal_inter_thread},
};

void run(unsigned int i)
{
	tcases[i].tfunc();
}

void setup(void)
{
	struct sigaction sigusr1 = {
		.sa_flags = SA_SIGINFO,
		.sa_sigaction = sigusr1_handler,
	};
	sigaction(SIGUSR1, &sigusr1, NULL);
}

void main(){
	setup();
	cleanup();
}
