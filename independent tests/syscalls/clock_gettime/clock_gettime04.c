#include "incl.h"
clockid_t clks[] = {
	CLOCK_REALTIME,
	CLOCK_REALTIME_COARSE,
	CLOCK_MONOTONIC,
	CLOCK_MONOTONIC_COARSE,
	CLOCK_MONOTONIC_RAW,
	CLOCK_BOOTTIME,
};

gettime_t ptr_vdso_gettime, ptr_vdso_gettime64;

long long delta, precise_delta, coarse_delta;

inline int do_vdso_gettime(gettime_t vdso, clockid_t clk_id, void *ts)
{
	if (!vdso) {
		errno = ENOSYS;
		return -1;
	}
	return vdso(clk_id, ts);
}

inline int vdso_gettime(clockid_t clk_id, void *ts)
{
	return do_vdso_gettime(ptr_vdso_gettime, clk_id, ts);
}

inline int vdso_gettime64(clockid_t clk_id, void *ts)
{
	return do_vdso_gettime(ptr_vdso_gettime64, clk_id, ts);
}

inline int my_gettimeofday(clockid_t clk_id, void *ts)
{
	struct timeval tval;
	if (clk_id != CLOCK_REALTIME)
		tst_brk(TBROK, "%s: Invalid clk_id, exiting", tst_clock_name(clk_id));
	if (gettimeofday(&tval, NULL) < 0)
		tst_brk(TBROK | TERRNO, "gettimeofday() failed");
	 * The array defines the type to TST_LIBC_TIMESPEC and so we can cast
	 * this into struct timespec.
	 */
	return 0;
}

struct time64_variants variants[] = {
	{ .clock_gettime = libc_clock_gettime, .ts_type = TST_LIBC_TIMESPEC, .desc = "vDSO or syscall with libc spec"},
#if (__NR_clock_gettime != __LTP__NR_INVALID_SYSCALL)
	{ .clock_gettime = sys_clock_gettime, .ts_type = TST_KERN_OLD_TIMESPEC, .desc = "syscall with old kernel spec"},
	{ .clock_gettime = vdso_gettime, .ts_type = TST_KERN_OLD_TIMESPEC, .desc = "vDSO with old kernel spec"},
#endif
#if (__NR_clock_gettime64 != __LTP__NR_INVALID_SYSCALL)
	{ .clock_gettime = sys_clock_gettime64, .ts_type = TST_KERN_TIMESPEC, .desc = "syscall time64 with kernel spec"},
	{ .clock_gettime = vdso_gettime64, .ts_type = TST_KERN_TIMESPEC, .desc = "vDSO time64 with kernel spec"},
#endif
	{ .clock_gettime = my_gettimeofday, .ts_type = TST_LIBC_TIMESPEC, .desc = "gettimeofday"},
};

void setup(void)
{
	struct timespec res;
	clock_getres(CLOCK_REALTIME, &res);
	precise_delta = 5 + res.tv_nsec / 1000000;
	clock_getres(CLOCK_REALTIME_COARSE, &res);
	coarse_delta = 5 + res.tv_nsec / 1000000;
	if (tst_is_virt(VIRT_ANY)) {
		tst_res(TINFO, "Running in a virtual machine, multiply the delta by 10.");
		precise_delta *= 10;
		coarse_delta *= 10;
	}
	find_clock_gettime_vdso(&ptr_vdso_gettime, &ptr_vdso_gettime64);
}

void run(unsigned int i)
{
	struct tst_ts ts;
	long long start, end = 0, diff, slack;
	struct time64_variants *tv;
	int count = 10000, ret;
	unsigned int j;
	if (clks[i] == CLOCK_REALTIME_COARSE || clks[i] == CLOCK_MONOTONIC_COARSE)
		delta = coarse_delta;
	else
		delta = precise_delta;
	do {
		for (j = 0; j < ARRAY_SIZE(variants); j++) {
			start = end;
			tv = &variants[j];
			ts.type = tv->ts_type;
			if (tv->clock_gettime == my_gettimeofday && clks[i] != CLOCK_REALTIME)
				continue;
			ret = tv->clock_gettime(clks[i], tst_ts_get(&ts));
			if (ret) {
				 * _vdso_gettime() sets error to ENOSYS if vdso
				 * isn't available.
				 */
				if (errno == ENOSYS)
					continue;
				tst_res(TFAIL | TERRNO, "%s: clock_gettime() failed (%d)",
					tst_clock_name(clks[i]), j);
				return;
			}
			end = tst_ts_to_ns(ts);
			if (count == 10000 && !j)
				continue;
			 * gettimeofday() doesn't capture time less than 1 us,
			 * add 999 to it.
			 */
			if (tv->clock_gettime == my_gettimeofday)
				slack = 999;
			else
				slack = 0;
			diff = end + slack - start;
			if (diff < 0) {
				tst_res(TFAIL, "%s: Time travelled backwards (%d): %lld ns",
					tst_clock_name(clks[i]), j, diff);
				return;
			}
			diff /= 1000000;
			if (diff >= delta) {
				tst_res(TFAIL, "%s(%s): Difference between successive readings greater than %lld ms (%d): %lld",
					tst_clock_name(clks[i]), tv->desc, delta, j, diff);
				return;
			}
		}
	} while (--count);
	tst_res(TPASS, "%s: Difference between successive readings is reasonable for following variants:",
			tst_clock_name(clks[i]));
	for (j = 0; j < ARRAY_SIZE(variants); j++) {
		if (variants[j].clock_gettime == my_gettimeofday && clks[i] != CLOCK_REALTIME)
			continue;
		tst_res(TINFO, "\t- %s", variants[j].desc);
	}
}

void main(){
	setup();
	cleanup();
}
