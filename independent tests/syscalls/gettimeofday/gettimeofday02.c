#include "incl.h"

volatile sig_atomic_t done;

void breakout(int sig)
{
	done = sig;
}

int  verify_gettimeofday(
{
	struct __kernel_old_timeval tv1, tv2;
	unsigned long long cnt = 0;
	int rtime = tst_remaining_runtime();
	done = 0;
	alarm(rtime);
	if (tst_syscall(__NR_gettimeofday, &tv1, NULL))
		tst_brk(TFAIL | TERRNO, "gettimeofday() failed");
	while (!done) {
		if (tst_syscall(__NR_gettimeofday, &tv2, NULL))
			tst_brk(TFAIL | TERRNO, "gettimeofday() failed");
		if (tv2.tv_sec < tv1.tv_sec ||
		    (tv2.tv_sec == tv1.tv_sec && tv2.tv_usec < tv1.tv_usec)) {
			tst_res(TFAIL,
				"Time is going backwards: old %jd.%jd vs new %jd.%jd!",
				(intmax_t) tv1.tv_sec, (intmax_t) tv1.tv_usec,
				(intmax_t) tv2.tv_sec, (intmax_t) tv2.tv_usec);
			return;
		}
		tv1 = tv2;
		cnt++;
	}
	tst_res(TINFO, "gettimeofday() called %llu times", cnt);
	tst_res(TPASS, "gettimeofday() monotonous in %i seconds", rtime);
}

void setup(void)
{
	signal(SIGALRM, breakout);
}

