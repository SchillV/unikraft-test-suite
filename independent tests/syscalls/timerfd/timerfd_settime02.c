#include "incl.h"
#define TIMERFD_FLAGS "timerfd_settime(TFD_TIMER_ABSTIME | TFD_TIMER_CANCEL_ON_SET)"
#ifndef TFD_TIMER_CANCEL_ON_SET
#define TFD_TIMER_CANCEL_ON_SET (1<<1)
#endif

int fd = -1;

struct tst_its its;

struct tst_fzsync_pair fzsync_pair;

struct time64_variants variants[] = {
#if (__NR_timerfd_settime != __LTP__NR_INVALID_SYSCALL)
	{ .tfd_settime = sys_timerfd_settime, .ts_type = TST_KERN_OLD_TIMESPEC, .desc = "syscall with old kernel spec"},
#endif
#if (__NR_timerfd_settime64 != __LTP__NR_INVALID_SYSCALL)
	{ .tfd_settime = sys_timerfd_settime64, .ts_type = TST_KERN_TIMESPEC, .desc = "syscall time64 with kernel spec"},
#endif
};

void setup(void)
{
	struct time64_variants *tv = &variants[tst_variant];
	tst_res(TINFO, "Testing variant: %s", tv->desc);
	its.type = tv->ts_type;
	fd = timerfd_create(CLOCK_REALTIME, 0);
	fzsync_pair.exec_loops = 1000000;
	tst_fzsync_pair_init(&fzsync_pair);
}

void cleanup(void)
{
	if (fd >= 0)
		close(fd);
	tst_fzsync_pair_cleanup(&fzsync_pair);
}

int punch_clock(int flags)
{
	return variants[tst_variant].tfd_settime(fd, flags, tst_its_get(&its),
						 NULL);
}

void *thread_run(void *arg)
{
	while (tst_fzsync_run_b(&fzsync_pair)) {
		tst_fzsync_start_race_b(&fzsync_pair);
		punch_clock(0);
		tst_fzsync_end_race_b(&fzsync_pair);
	}
	return arg;
}

void run(void)
{
	tst_fzsync_pair_reset(&fzsync_pair, thread_run);
	while (tst_fzsync_run_a(&fzsync_pair)) {
punch_clock(TFD_TIMER_ABSTIME | TFD_TIMER_CANCEL_ON_SET);
		if (TST_RET == -1)
			tst_brk(TBROK | TTERRNO, TIMERFD_FLAGS " failed");
		if (TST_RET != 0)
			tst_brk(TBROK | TTERRNO, "Invalid " TIMERFD_FLAGS
				" return value");
		tst_fzsync_start_race_a(&fzsync_pair);
		punch_clock(0);
		tst_fzsync_end_race_a(&fzsync_pair);
		if (tst_taint_check()) {
			tst_res(TFAIL, "Kernel is vulnerable");
			return;
		}
	}
	tst_res(TPASS, "Nothing bad happened, probably");
}

void main(){
	setup();
	cleanup();
}
