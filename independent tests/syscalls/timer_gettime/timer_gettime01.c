#include "incl.h"

struct time64_variants variants[] = {
#if (__NR_timer_gettime != __LTP__NR_INVALID_SYSCALL)
	{ .timer_gettime = sys_timer_gettime, .ts_type = TST_KERN_OLD_TIMESPEC, .desc = "syscall with old kernel spec"},
#endif
#if (__NR_timer_gettime64 != __LTP__NR_INVALID_SYSCALL)
	{ .timer_gettime = sys_timer_gettime64, .ts_type = TST_KERN_TIMESPEC, .desc = "syscall time64 with kernel spec"},
#endif
};

kernel_timer_t timer;

void setup(void)
{
	struct sigevent ev;
	tst_res(TINFO, "Testing variant: %s", variants[tst_variant].desc);
	ev.sigev_value = (union sigval) 0;
	ev.sigev_signo = SIGALRM;
	ev.sigev_notify = SIGEV_SIGNAL;
tst_syscall(__NR_timer_create, CLOCK_REALTIME, &ev, &timer);
	if (TST_RET) {
		tst_res(TFAIL | TTERRNO, "timer_create() failed");
		return;
	}
}

int  verify(
{
	struct time64_variants *tv = &variants[tst_variant];
	struct tst_its spec = {.type = tv->ts_type, };
tv->timer_gettime(timer, tst_its_get(&spec));
	if (TST_RET == 0) {
		if (tst_its_get_interval_sec(spec) ||
		    tst_its_get_interval_nsec(spec) ||
		    tst_its_get_value_sec(spec) ||
		    tst_its_get_value_nsec(spec))
			tst_res(TFAIL, "timespec should have been zeroed");
		else
			tst_res(TPASS, "timer_gettime() Passed");
	} else {
		tst_res(TFAIL | TTERRNO, "timer_gettime() Failed");
	}
tv->timer_gettime((kernel_timer_t)-1, tst_its_get(&spec));
	if (TST_RET == -1 && TST_ERR == EINVAL)
		tst_res(TPASS, "timer_gettime(-1) Failed: EINVAL");
	else
		tst_res(TFAIL | TTERRNO, "timer_gettime(-1) = %li", TST_RET);
tv->timer_gettime(timer, NULL);
	if (TST_RET == -1 && TST_ERR == EFAULT)
		tst_res(TPASS, "timer_gettime(NULL) Failed: EFAULT");
	else
		tst_res(TFAIL | TTERRNO, "timer_gettime(-1) = %li", TST_RET);
}

void main(){
	setup();
	cleanup();
}
