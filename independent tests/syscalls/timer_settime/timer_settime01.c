#include "incl.h"

struct tst_ts timenow;

struct tst_its new_set, old_set;

kernel_timer_t timer;

struct testcase {
	struct tst_its		*old_ptr;
	int			it_value_tv_usec;
	int			it_interval_tv_usec;
	int			flag;
	char			*description;
} tcases[] = {
	{NULL, 50000, 0, 0, "general initialization"},
	{&old_set, 50000, 0, 0, "setting old_value"},
	{&old_set, 50000, 50000, 0, "using periodic timer"},
	{&old_set, 50000, 0, TIMER_ABSTIME, "using absolute time"},
};

struct time64_variants variants[] = {
#if (__NR_timer_settime != __LTP__NR_INVALID_SYSCALL)
	{ .clock_gettime = sys_clock_gettime, .timer_gettime = sys_timer_gettime, .timer_settime = sys_timer_settime, .ts_type = TST_KERN_OLD_TIMESPEC, .desc = "syscall with old kernel spec"},
#endif
#if (__NR_timer_settime64 != __LTP__NR_INVALID_SYSCALL)
	{ .clock_gettime = sys_clock_gettime64, .timer_gettime = sys_timer_gettime64, .timer_settime = sys_timer_settime64, .ts_type = TST_KERN_TIMESPEC, .desc = "syscall time64 with kernel spec"},
#endif
};

volatile int caught_signal;

void clear_signal(void)
{
	 * The busy loop is intentional. The signal is sent after X
	 * seconds of CPU time has been accumulated for the process and
	 * thread specific clocks.
	 */
	while (!caught_signal);
	if (caught_signal != SIGALRM) {
		tst_res(TFAIL, "Received incorrect signal: %s",
			tst_strsig(caught_signal));
	}
	caught_signal = 0;
}

void sighandler(int sig)
{
	caught_signal = sig;
}

void setup(void)
{
	tst_res(TINFO, "Testing variant: %s", variants[tst_variant].desc);
	signal(SIGALRM, sighandler);
}

void run(unsigned int n)
{
	struct time64_variants *tv = &variants[tst_variant];
	struct testcase *tc = &tcases[n];
	long long val;
	unsigned int i;
	tst_res(TINFO, "Testing for %s:", tc->description);
	for (i = 0; i < CLOCKS_DEFINED; ++i) {
		clock_t clock = clock_list[i];
tst_syscall(__NR_timer_create, clock, NULL, &timer);
		if (TST_RET != 0) {
			if (possibly_unsupported(clock) &&
				(TST_ERR == EINVAL || TST_ERR == ENOTSUP)) {
				tst_res(TCONF | TTERRNO, "%s unsupported",
					get_clock_str(clock));
			} else {
				tst_res(TFAIL | TTERRNO,
					"timer_create(%s) failed",
					get_clock_str(clock));
			}
			continue;
		}
		memset(&new_set, 0, sizeof(new_set));
		memset(&old_set, 0, sizeof(old_set));
		new_set.type = old_set.type = tv->ts_type;
		val = tc->it_value_tv_usec;
		if (tc->flag & TIMER_ABSTIME) {
			timenow.type = tv->ts_type;
			if (tv->clock_gettime(clock, tst_ts_get(&timenow)) < 0) {
				tst_res(TFAIL,
					"clock_gettime(%s) failed - skipping the test",
					get_clock_str(clock));
				continue;
			}
			tst_ts_add_us(timenow, val);
			tst_its_set_value_from_ts(&new_set, timenow);
		} else {
			tst_its_set_value_from_us(&new_set, val);
		}
		tst_its_set_interval_from_us(&new_set, tc->it_interval_tv_usec);
tv->timer_settime(timer, tc->flag, tst_its_get(&new_set), tst_its_get(tc->old_ptr));
		if (TST_RET != 0) {
			tst_res(TFAIL | TTERRNO, "timer_settime(%s) failed",
				get_clock_str(clock));
		}
tv->timer_gettime(timer, tst_its_get(&new_set));
		if (TST_RET != 0) {
			tst_res(TFAIL | TTERRNO, "timer_gettime(%s) failed",
				get_clock_str(clock));
		} else if ((tst_its_get_interval_nsec(new_set) !=
				tc->it_interval_tv_usec * 1000) ||
			   (tst_its_get_value_nsec(new_set) >
				MAX(tc->it_value_tv_usec * 1000, tc->it_interval_tv_usec * 1000))) {
			tst_res(TFAIL | TTERRNO,
				"timer_gettime(%s) reported bad values (%llu: %llu)",
				get_clock_str(clock),
				tst_its_get_interval_nsec(new_set),
				tst_its_get_value_nsec(new_set));
		}
		clear_signal();
		if (tc->it_interval_tv_usec)
			clear_signal();
		tst_res(TPASS, "timer_settime(%s) passed",
			get_clock_str(clock));
tst_syscall(__NR_timer_delete, timer);
		if (TST_RET != 0)
			tst_res(TFAIL | TTERRNO, "timer_delete() failed!");
	}
}

void main(){
	setup();
	cleanup();
}
