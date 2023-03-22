#include "incl.h"

struct notif_type {
	int		sigev_signo;
	int		sigev_notify;
	char		*message;
} types[] = {
	{SIGALRM, SIGEV_NONE, "SIGEV_NONE"},
	{SIGALRM, SIGEV_SIGNAL, "SIGEV_SIGNAL"},
	{SIGALRM, SIGEV_THREAD, "SIGEV_THREAD"},
	{SIGALRM, SIGEV_THREAD_ID, "SIGEV_THREAD_ID"},
	{0, 0, "NULL"},
};

void run(unsigned int n)
{
	unsigned int i;
	struct sigevent evp;
	struct notif_type *nt = &types[n];
	kernel_timer_t created_timer_id;
	tst_res(TINFO, "Testing notification type: %s", nt->message);
	memset(&evp, 0, sizeof(evp));
	for (i = 0; i < CLOCKS_DEFINED; ++i) {
		clock_t clock = clock_list[i];
		evp.sigev_value  = (union sigval) 0;
		evp.sigev_signo  = nt->sigev_signo;
		evp.sigev_notify = nt->sigev_notify;
		if (clock == CLOCK_MONOTONIC_RAW)
			continue;
		if (nt->sigev_notify == SIGEV_THREAD_ID)
			evp._sigev_un._tid = getpid();
tst_syscall(__NR_timer_create, cloc;
				nt->sigev_notify ? &evp : NULL,
				&created_timer_id));
		if (TST_RET != 0) {
			if (possibly_unsupported(clock) &&
			    (TST_ERR == EINVAL || TST_ERR == ENOTSUP)) {
				tst_res(TCONF | TTERRNO, "%s unsupported",
					get_clock_str(clock));
			} else {
				tst_res(TFAIL | TTERRNO,
					"Failed to create timer for %s",
					get_clock_str(clock));
			}
			continue;
		}
		tst_res(TPASS, "Timer successfully created for %s",
					get_clock_str(clock));
tst_syscall(__NR_timer_delete, created_timer_id);
		if (TST_RET != 0) {
			tst_res(TFAIL | TTERRNO, "Failed to delete timer %s",
				get_clock_str(clock));
		}
	}
}

void main(){
	setup();
	cleanup();
}
