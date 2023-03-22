#include "incl.h"
#define PROC_SCHED_RR_TIMESLICE_MS	"/proc/sys/kernel/sched_rr_timeslice_ms"

int proc_flag;
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
	if ((sys_sched_setscheduler(0, SCHED_RR, &p)) == -1)
		tst_res(TFAIL | TERRNO, "sched_setscheduler() failed");
	proc_flag = !access(PROC_SCHED_RR_TIMESLICE_MS, F_OK);
}

void run(void)
{
	struct time64_variants *tv = &variants[tst_variant];
tv->sched_rr_get_interval(0, tst_ts_get(&tp));
	if (!TST_RET) {
		tst_res(TPASS, "sched_rr_get_interval() passed");
	} else {
		tst_res(TFAIL | TTERRNO, "Test Failed, sched_rr_get_interval() returned %ld",
			TST_RET);
	}
	if (!tst_ts_valid(&tp)) {
		tst_res(TPASS, "Time quantum %llis %llins",
		        tst_ts_get_sec(tp), tst_ts_get_nsec(tp));
	} else {
		tst_res(TFAIL, "Invalid time quantum %llis %llins",
		        tst_ts_get_sec(tp), tst_ts_get_nsec(tp));
	}
	if (proc_flag)
		TST_ASSERT_INT("/proc/sys/kernel/sched_rr_timeslice_ms", tst_ts_to_ms(tp));
}

void main(){
	setup();
	cleanup();
}
