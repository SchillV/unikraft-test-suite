#include "incl.h"
#define _GNU_SOURCE

struct tcase {
	int id;
	char const *name;
} tcases[] = {
	{CLOCK_MONOTONIC, "CLOCK MONOTONIC"},
	{CLOCK_REALTIME, "CLOCK REALTIME"},
};

struct time64_variants variants[] = {
#if (__NR_timerfd_gettime != __LTP__NR_INVALID_SYSCALL)
	{ .clock_gettime = sys_clock_gettime, .tfd_gettime = sys_timerfd_gettime, .tfd_settime = sys_timerfd_settime, .ts_type = TST_KERN_OLD_TIMESPEC, .desc = "syscall with old kernel spec"},
#endif
#if (__NR_timerfd_gettime64 != __LTP__NR_INVALID_SYSCALL)
	{ .clock_gettime = sys_clock_gettime64, .tfd_gettime = sys_timerfd_gettime64, .tfd_settime = sys_timerfd_settime64, .ts_type = TST_KERN_TIMESPEC, .desc = "syscall time64 with kernel spec"},
#endif
};

unsigned long long getustime(int clockid)
{
	struct time64_variants *tv = &variants[tst_variant];
	struct tst_ts tp = {.type = tv->ts_type, };
	if (tv->clock_gettime((clockid_t) clockid, tst_ts_get(&tp))) {
		tst_res(TFAIL | TERRNO, "clock_gettime() failed");
		return 0;
	}
	return 1000000ULL * tst_ts_get_sec(tp) + tst_ts_get_nsec(tp) / 1000;
}

void settime(int tfd, struct tst_its *tmr, int tflags,
                    unsigned long long tvalue, int tinterval)
{
	struct time64_variants *tv = &variants[tst_variant];
	tst_its_set_value_from_us(tmr, tvalue);
	tst_its_set_interval_from_us(tmr, tinterval);
	if (tv->tfd_settime(tfd, tflags, tst_its_get(tmr), NULL))
		tst_res(TFAIL | TERRNO, "timerfd_settime() failed");
}

void waittmr(int tfd, unsigned int exp_ticks)
{
	uint64_t ticks;
	struct pollfd pfd;
	pfd.fd = tfd;
	pfd.events = POLLIN;
	pfd.revents = 0;
	if (poll(&pfd, 1, -1) < 0) {
		tst_res(TFAIL | TERRNO, "poll() failed");
		return;
	}
	if ((pfd.revents & POLLIN) == 0) {
		tst_res(TFAIL, "no ticks happened");
		return;
	}
	read(0, tfd, &ticks, sizeof0, tfd, &ticks, sizeofticks));
	if (ticks != exp_ticks) {
		tst_res(TFAIL, "got %u tick(s) expected %u",
		        (unsigned int)ticks, exp_ticks);
	} else {
		tst_res(TPASS, "got %u tick(s)", exp_ticks);
	}
}

void run(unsigned int n)
{
	struct time64_variants *tv = &variants[tst_variant];
	int tfd;
	unsigned long long tnow;
	uint64_t uticks;
	struct tst_its tmr = {.type = tv->ts_type, };
	struct tcase *clks = &tcases[n];
	tst_res(TINFO, "testing %s", clks->name);
	tfd = timerfd_create(clks->id, 0);
	tst_res(TINFO, "relative timer (100 ms)");
	settime(tfd, &tmr, 0, 100 * 1000, 0);
	waittmr(tfd, 1);
	tst_res(TINFO, "absolute timer (100 ms)");
	tnow = getustime(clks->id);
	settime(tfd, &tmr, TFD_TIMER_ABSTIME, tnow + 100 * 1000, 0);
	waittmr(tfd, 1);
	tst_res(TINFO, "sequential timer (50 ms)");
	tnow = getustime(clks->id);
	settime(tfd, &tmr, TFD_TIMER_ABSTIME, tnow + 50 * 1000, 50 * 1000);
	memset(&tmr, 0, sizeof(tmr));
	tmr.type = tv->ts_type;
	if (tv->tfd_gettime(tfd, tst_its_get(&tmr)))
		tst_res(TFAIL | TERRNO, "timerfd_gettime() failed");
	if (tst_its_get_value_sec(tmr) != 0 || tst_its_get_value_nsec(tmr) > 50 * 1000000)
		tst_res(TFAIL, "Timer read back value not relative");
	else
		tst_res(TPASS, "Timer read back value is relative");
	usleep(160000);
	waittmr(tfd, 3);
	tst_res(TINFO, "testing with O_NONBLOCK");
	settime(tfd, &tmr, 0, 100 * 1000, 0);
	waittmr(tfd, 1);
	fcntl(tfd, F_SETFL, fcntltfd, F_SETFL, fcntltfd, F_GETFL, 0) | O_NONBLOCK);
read(tfd, &uticks, sizeof(uticks));
	if (TST_RET > 0)
		tst_res(TFAIL, "timer ticks not zero");
	else if (TST_ERR != EAGAIN)
		tst_res(TFAIL | TTERRNO, "read should fail with EAGAIN got");
	else
		tst_res(TPASS | TERRNO, "read failed with");
	close(tfd);
}

void setup(void)
{
	tst_res(TINFO, "Testing variant: %s", variants[tst_variant].desc);
}

void main(){
	setup();
	cleanup();
}
