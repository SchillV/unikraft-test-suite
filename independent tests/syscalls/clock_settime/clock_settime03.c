#include "incl.h"
#define TIMER_DELTA	3

struct tst_ts start, end;

struct tst_its its;

struct time64_variants variants[] = {
#if (__NR_clock_settime != __LTP__NR_INVALID_SYSCALL)
	{ .clock_gettime = sys_clock_gettime, .clock_settime = sys_clock_settime, .timer_settime = sys_timer_settime, .ts_type = TST_KERN_OLD_TIMESPEC, .desc = "syscall with old kernel spec"},
#endif
#if (__NR_clock_settime64 != __LTP__NR_INVALID_SYSCALL)
	{ .clock_gettime = sys_clock_gettime64, .clock_settime = sys_clock_settime64, .timer_settime = sys_timer_settime64, .ts_type = TST_KERN_TIMESPEC, .desc = "syscall time64 with kernel spec"},
#endif
};

void setup(void)
{
	struct time64_variants *tv = &variants[tst_variant];
	tst_res(TINFO, "Testing variant: %s", tv->desc);
	start.type = end.type = its.type = tv->ts_type;
	if (tv->ts_type == TST_KERN_OLD_TIMESPEC &&
	    sizeof(start.ts.kern_old_ts.tv_sec) == 4) {
		tst_brk(TCONF, "Not Y2038 safe to run test");
	}
}

void run(void)
{
	struct time64_variants *tv = &variants[tst_variant];
	struct sigevent ev = {
		.sigev_notify = SIGEV_SIGNAL,
		.sigev_signo = SIGABRT,
	};
	long long diff;
	kernel_timer_t timer;
	sigset_t set;
	int sig, ret;
	sigemptyset(&set);
	sigaddset(&set, SIGABRT);
	sigprocmask(SIG_BLOCK, &set, NULL);
tst_syscall(__NR_timer_create, CLOCK_REALTIME, &ev, &timer);
	if (TST_RET != 0)
		tst_brk(TBROK | TTERRNO, "timer_create() failed");
	tst_ts_set_sec(&start, time);
	tst_ts_set_nsec(&start, 0);
	ret = tv->clock_settime(CLOCK_REALTIME, tst_ts_get(&start));
	if (ret == -1)
		tst_brk(TBROK | TERRNO, "clock_settime() failed");
	tst_its_set_interval_sec(&its, 0);
	tst_its_set_interval_nsec(&its, 0);
	tst_its_set_value_sec(&its, time + TIMER_DELTA);
	tst_its_set_value_nsec(&its, 0);
tv->timer_settime(timer, TIMER_ABSTIME, tst_its_get(&its), NULL);
	if (TST_RET == -1)
		tst_brk(TBROK | TTERRNO, "timer_settime() failed");
	sigwait(&set, &sig);
	ret = tv->clock_gettime(CLOCK_REALTIME, tst_ts_get(&end));
	if (ret == -1)
		tst_brk(TBROK | TERRNO, "clock_gettime() failed");
	if (sig != SIGABRT) {
		tst_res(TFAIL, "clock_settime(): Y2038 test failed");
		return;
	}
	diff = tst_ts_diff_ms(end, start);
	if (diff < TIMER_DELTA * 1000) {
		tst_res(TFAIL, "Timer expired too soon, after %llims", diff);
		return;
	}
	if (diff > TIMER_DELTA * 1000 + ALLOWED_DELTA)
		tst_res(TINFO, "Time expired too late, after %llims", diff);
	else
		tst_res(TPASS, "clock_settime(): Y2038 test passed");
}

void main(){
	setup();
	cleanup();
}
