#include "incl.h"

void do_child(void)
{
	struct timespec timereq = {.tv_sec = 5, .tv_nsec = 9999};
	struct timespec timerem, exp_rem;
	tst_timer_start(CLOCK_MONOTONIC);
nanosleep(&timereq, &timerem);
	tst_timer_stop();
	if (TST_RET != -1) {
		tst_res(TFAIL,
			"nanosleep was not interrupted, returned %ld, expected -1",
			TST_RET);
		return;
	}
	if (TST_ERR != EINTR) {
		tst_res(TFAIL | TTERRNO,
			"nanosleep() failed, expected EINTR, got");
		return;
	}
	tst_res(TPASS, "nanosleep() returned -1, set errno to EINTR");
	if (tst_timespec_lt(timereq, tst_timer_elapsed()))
		tst_brk(TFAIL, "nanosleep() slept more than timereq");
	exp_rem = tst_timespec_diff(timereq, tst_timer_elapsed());
	if (tst_timespec_abs_diff_us(timerem, exp_rem) > USEC_PRECISION) {
		tst_res(TFAIL,
			"nanosleep() remaining time %llius, expected %llius, diff %llius",
			tst_timespec_to_us(timerem), tst_timespec_to_us(exp_rem),
			tst_timespec_abs_diff_us(timerem, exp_rem));
	} else {
		tst_res(TPASS,
			"nanosleep() slept for %llius, remaining time difference %llius",
			tst_timer_elapsed_us(),
			tst_timespec_abs_diff_us(timerem, exp_rem));
	}
}
void run(void)
{
	pid_t cpid;
	cpid = fork();
	if (cpid == 0) {
		do_child();
	} else {
		sleep(1);
		kill(cpid, SIGINT);
		tst_reap_children();
	}
}

void sig_handler(int si LTP_ATTRIBUTE_UNUSED)
{
}

void setup(void)
{
	tst_timer_check(CLOCK_MONOTONIC);
	signal(SIGINT, sig_handler);
}

