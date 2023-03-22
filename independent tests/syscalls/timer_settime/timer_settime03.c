#include "incl.h"

timer_t timer;

volatile int handler_called, overrun, saved_errno;

struct timespec realtime_resolution;

void sighandler(int sig LTP_ATTRIBUTE_UNUSED)
{
	struct itimerspec spec;
	 * Signal handler will be called twice in total because kernel will
	 * schedule another pending signal before the timer gets disabled.
	 */
	if (handler_called)
		return;
	errno = 0;
	overrun = timer_getoverrun(timer);
	saved_errno = errno;
	memset(&spec, 0, sizeof(struct itimerspec));
	timer_settime(timer, 0, &spec, NULL);
	handler_called = 1;
}

void setup(void)
{
	struct sigevent sev;
	memset(&sev, 0, sizeof(struct sigevent));
	sev.sigev_notify = SIGEV_SIGNAL;
	sev.sigev_signo = SIGUSR1;
	signal(SIGUSR1, sighandler);
	timer_create(CLOCK_REALTIME, &sev, &timer);
	clock_getres(CLOCK_REALTIME, &realtime_resolution);
	tst_res(TINFO, "CLOCK_REALTIME resolution %lins",
	        (long)realtime_resolution.tv_nsec);
}

void run(void)
{
	int handler_delay = INT_MAX / 7;
	long nsec;
	struct itimerspec spec;
	handler_called = 0;
	memset(&spec, 0, sizeof(struct itimerspec));
	clock_gettime(CLOCK_REALTIME, &spec.it_value);
	nsec = (handler_delay % 100000000) * 10L;
	if (nsec > spec.it_value.tv_nsec) {
		spec.it_value.tv_sec -= 1;
		spec.it_value.tv_nsec += 1000000000;
	}
	spec.it_value.tv_sec -= (handler_delay / 100000000) * realtime_resolution.tv_nsec;
	spec.it_value.tv_nsec -= nsec;
	spec.it_interval.tv_nsec = realtime_resolution.tv_nsec;
	timer_settime(timer, TIMER_ABSTIME, &spec, NULL);
	while (!handler_called);
	errno = saved_errno;
	if (overrun == -1)
		tst_brk(TBROK | TERRNO, "Error reading timer overrun count");
	if (overrun == INT_MAX) {
		tst_res(TPASS, "Timer overrun count is capped");
		return;
	}
	if (overrun < 0) {
		tst_res(TFAIL, "Timer overrun counter overflow");
		return;
	}
	tst_res(TFAIL, "Timer overrun counter is wrong: %d; expected %d or "
		"negative number", overrun, INT_MAX);
}

void cleanup(void)
{
	timer_delete(timer);
}

void main(){
	setup();
	cleanup();
}
