#include "incl.h"
#define RANDOM_UNUSED_NUMBER (54321)

void run(void)
{
	struct sigevent evp;
	clock_t clock = CLOCK_MONOTONIC;
	kernel_timer_t created_timer_id;
	memset(&evp, 0, sizeof(evp));
	evp.sigev_signo  = SIGALRM;
	evp.sigev_notify = SIGEV_SIGNAL | RANDOM_UNUSED_NUMBER;
	evp._sigev_un._tid = getpid();
tst_syscall(__NR_timer_create, clock, &evp, &created_timer_id);
	if (TST_RET != 0) {
		if (TST_ERR == EINVAL) {
			tst_res(TPASS | TTERRNO,
					"timer_create() failed as expected");
		} else {
			tst_res(TFAIL | TTERRNO,
					"timer_create() unexpectedly failed");
		}
		return;
	}
	tst_res(TFAIL,
		"timer_create() succeeded for invalid notification type");
tst_syscall(__NR_timer_delete, created_timer_id);
	if (TST_RET != 0) {
		tst_res(TFAIL | TTERRNO, "Failed to delete timer %s",
			get_clock_str(clock));
	}
}

void main(){
	setup();
	cleanup();
}
