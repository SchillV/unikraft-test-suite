#include "incl.h"

struct tst_ts tp;

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
	tp.type = tv->ts_type;
	if ((sys_sched_setscheduler(0, SCHED_FIFO, &p)) == -1)
		tst_res(TFAIL | TERRNO, "sched_setscheduler() failed");
}

void run(void)
{
	struct time64_variants *tv = &variants[tst_variant];
	tst_ts_set_sec(&tp, 99);
	tst_ts_set_nsec(&tp, 99);
tv->sched_rr_get_interval(0, tst_ts_get(&tp));
	if (!TST_RET && tst_ts_valid(&tp) == -1) {
		tst_res(TPASS, "sched_rr_get_interval() passed");
	} else {
		tst_res(TFAIL | TTERRNO,
			"sched_rr_get_interval() returned %ld, tp.tv_sec = %lld, tp.tv_nsec = %lld",
			TST_RET, tst_ts_get_sec(tp), tst_ts_get_nsec(tp));
	}
}

void main(){
	setup();
	cleanup();
}
