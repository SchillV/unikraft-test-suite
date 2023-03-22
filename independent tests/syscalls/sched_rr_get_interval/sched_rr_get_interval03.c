#include "incl.h"

pid_t unused_pid;

pid_t inval_pid = -1;

pid_t zero_pid;

struct tst_ts tp;

void *bad_addr;
struct test_cases_t {
	pid_t *pid;
	struct tst_ts *tp;
	int exp_errno;
} test_cases[] = {
	{ &inval_pid, &tp, EINVAL},
	{ &unused_pid, &tp, ESRCH},
	{ &zero_pid, NULL, EFAULT}
};

struct time64_variants variants[] = {
	{ .sched_rr_get_interval = libc_sched_rr_get_interval, .ts_type = TST_LIBC_TIMESPEC, .desc = "vDSO or syscall with libc spec"},
#if (__NR_sched_rr_get_interval != __LTP__NR_INVALID_SYSCALL)
	{ .sched_rr_get_interval = sys_sched_rr_get_interval, .ts_type = TST_KERN_OLD_TIMESPEC, .desc = "syscall with old kernel spec"},
#endif
#if (__NR_sched_rr_get_interval_time64 != __LTP__NR_INVALID_SYSCALL)
	{ .sched_rr_get_interval = sys_sched_rr_get_interval64, .ts_type = TST_KERN_TIMESPEC, .desc = "syscall time64 with kernel spec"},
#endif
};

void setup(void)
{
	struct time64_variants *tv = &variants[tst_variant];
	struct sched_param p = { 1 };
	tst_res(TINFO, "Testing variant: %s", tv->desc);
	bad_addr = tst_get_bad_addr(NULL);
	tp.type = tv->ts_type;
	if ((sys_sched_setscheduler(0, SCHED_RR, &p)) == -1)
		tst_res(TFAIL | TERRNO, "sched_setscheduler() failed");
	unused_pid = tst_get_unused_pid();
}

void run(unsigned int i)
{
	struct time64_variants *tv = &variants[tst_variant];
	struct test_cases_t *tc = &test_cases[i];
	struct timerspec *ts;
	if (tc->exp_errno == EFAULT
		&& tv->sched_rr_get_interval == libc_sched_rr_get_interval) {
		tst_res(TCONF, "EFAULT skipped for libc_variant");
		return;
	}
	if (tc->exp_errno == EFAULT)
		ts = bad_addr;
	else
		ts = tst_ts_get(tc->tp);
	TST_EXP_FAIL(tv->sched_rr_get_interval(*tc->pid, ts), tc->exp_errno,
	             "sched_rr_get_interval(%i, %p)", *tc->pid, ts);
}

void main(){
	setup();
	cleanup();
}
