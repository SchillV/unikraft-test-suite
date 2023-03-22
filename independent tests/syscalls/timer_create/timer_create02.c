#include "incl.h"

struct sigevent sig_ev = {
	.sigev_notify = SIGEV_NONE,
	.sigev_signo  = SIGALRM,
};

struct sigevent sig_ev_inv_not = {
	.sigev_notify = INT_MAX,
	.sigev_signo = SIGALRM,
};

struct sigevent sig_ev_inv_sig = {
	.sigev_notify = SIGEV_SIGNAL,
	.sigev_signo = INT_MAX,
};

kernel_timer_t timer_id;

struct testcase {
	clock_t clock;
	struct sigevent	*ev_ptr;
	kernel_timer_t	*kt_ptr;
	int		error;
	char		*desc;
} tcases[] = {
	{CLOCK_REALTIME, NULL, &timer_id, EFAULT, "invalid sigevent struct"},
	{CLOCK_REALTIME, &sig_ev, NULL, EFAULT, "invalid timer ID"},
	{MAX_CLOCKS, &sig_ev, &timer_id, EINVAL, "invalid clock"},
	{CLOCK_REALTIME, &sig_ev_inv_not, &timer_id, EINVAL, "wrong sigev_notify"},
	{CLOCK_REALTIME, &sig_ev_inv_sig, &timer_id, EINVAL, "wrong sigev_signo"},
};

void run(unsigned int n)
{
	struct testcase *tc = &tcases[n];
tst_syscall(__NR_timer_create, tc->clock, tc->ev_ptr, tc->kt_ptr);
	if (TST_RET != -1 || TST_ERR != tc->error) {
		tst_res(TFAIL | TTERRNO,
			"%s idn't fail as expected (%s) - Got",
			tc->desc, tst_strerrno(tc->error));
		return;
	}
	tst_res(TPASS | TTERRNO, "%s failed as expected", tc->desc);
}

void setup(void)
{
	unsigned int i;
	for (i = 0; i < ARRAY_SIZE(tcases); i++) {
		if (!tcases[i].ev_ptr)
			tcases[i].ev_ptr = tst_get_bad_addr(NULL);
		if (!tcases[i].kt_ptr)
			tcases[i].kt_ptr = tst_get_bad_addr(NULL);
	}
}

void main(){
	setup();
	cleanup();
}
