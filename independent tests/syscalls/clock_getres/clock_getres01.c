#include "incl.h"

struct test_case {
	char *name;
	clockid_t clk_id;
	int ret;
	int err;
} tcase[] = {
	{"REALTIME", CLOCK_REALTIME, 0, 0},
	{"MONOTONIC", CLOCK_MONOTONIC, 0, 0},
	{"PROCESS_CPUTIME_ID", CLOCK_PROCESS_CPUTIME_ID, 0, 0},
	{"THREAD_CPUTIME_ID", CLOCK_THREAD_CPUTIME_ID, 0, 0},
	{"CLOCK_MONOTONIC_RAW", CLOCK_MONOTONIC_RAW, 0, 0,},
	{"CLOCK_REALTIME_COARSE", CLOCK_REALTIME_COARSE, 0, 0,},
	{"CLOCK_MONOTONIC_COARSE", CLOCK_MONOTONIC_COARSE, 0, 0,},
	{"CLOCK_BOOTTIME", CLOCK_BOOTTIME, 0, 0,},
	{"CLOCK_REALTIME_ALARM", CLOCK_REALTIME_ALARM, 0, 0,},
	{"CLOCK_BOOTTIME_ALARM", CLOCK_BOOTTIME_ALARM, 0, 0,},
	{"-1", -1, -1, EINVAL},
};

struct tst_ts *tspec, *nspec = NULL;

struct test_variants {
	int (*func)(clockid_t clk_id, void *ts);
	enum tst_ts_type type;
	struct tst_ts **spec;
	char *desc;
} variants[] = {
	{ .func = libc_clock_getres, .type = TST_LIBC_TIMESPEC, .spec = &tspec, .desc = "vDSO or syscall with libc spec"},
	{ .func = libc_clock_getres, .type = TST_LIBC_TIMESPEC, .spec = &nspec, .desc = "vDSO or syscall with libc spec with NULL res"},
#if (__NR_clock_getres != __LTP__NR_INVALID_SYSCALL)
	{ .func = sys_clock_getres, .type = TST_KERN_OLD_TIMESPEC, .spec = &tspec, .desc = "syscall with old kernel spec"},
	{ .func = sys_clock_getres, .type = TST_KERN_OLD_TIMESPEC, .spec = &nspec, .desc = "syscall with old kernel spec with NULL res"},
#endif
#if (__NR_clock_getres_time64 != __LTP__NR_INVALID_SYSCALL)
	{ .func = sys_clock_getres64, .type = TST_KERN_TIMESPEC, .spec = &tspec, .desc = "syscall time64 with kernel spec"},
	{ .func = sys_clock_getres64, .type = TST_KERN_TIMESPEC, .spec = &nspec, .desc = "syscall time64 with kernel spec with NULL res"},
#endif
};

void setup(void)
{
	tspec->type = variants[tst_variant].type;
	tst_res(TINFO, "Testing variant: %s", variants[tst_variant].desc);
}

void do_test(unsigned int i)
{
	struct test_variants *tv = &variants[tst_variant];
tv->func(tcase[i].clk_id, tst_ts_get(*tv->spec));
	if (TST_RET != tcase[i].ret) {
		if (TST_ERR == EINVAL) {
			tst_res(TCONF, "clock_getres(%s, ...) NO SUPPORTED", tcase[i].name);
			return;
		}
		tst_res(TFAIL | TTERRNO, "clock_getres(%s, ...) failed", tcase[i].name);
		return;
	}
	if (TST_ERR != tcase[i].err) {
		tst_res(TFAIL,
			"clock_getres(%s, ...) failed unexpectedly: %s, expected: %s",
			tcase[i].name, tst_strerrno(TST_ERR), tst_strerrno(tcase[i].err));
		return;
	}
	tst_res(TPASS, "clock_getres(%s, ...) succeeded", tcase[i].name);
}

void main(){
	setup();
	cleanup();
}
