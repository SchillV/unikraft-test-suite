#include "incl.h"

struct timeval tv;

struct itimerval *value, *ovalue;

volatile unsigned long sigcnt;

long time_step;

long time_sec;

long time_usec;

struct tcase {
	int which;
	char *des;
	int signo;
} tcases[] = {
	{ITIMER_REAL,    "ITIMER_REAL",    SIGALRM},
	{ITIMER_VIRTUAL, "ITIMER_VIRTUAL", SIGVTALRM},
	{ITIMER_PROF,    "ITIMER_PROF",    SIGPROF},
};

int sys_setitimer(int which, void *new_value, void *old_value)
{
	return tst_syscall(__NR_setitimer, which, new_value, old_value);
}

void sig_routine(int signo LTP_ATTRIBUTE_UNUSED)
{
	sigcnt++;
}

void set_setitimer_value(int sec, int usec)
{
	value->it_value.tv_sec = sec;
	value->it_value.tv_usec = usec;
	value->it_interval.tv_sec = sec;
	value->it_interval.tv_usec = usec;
}

int  verify_setitimer(unsigned int i)
{
	pid_t pid;
	int status;
	long margin;
	struct tcase *tc = &tcases[i];
	tst_res(TINFO, "tc->which = %s", tc->des);
	if (tc->which == ITIMER_REAL) {
		if (gettimeofday(&tv, NULL) == -1)
			tst_brk(TBROK | TERRNO, "gettimeofday(&tv1, NULL) failed");
		else
			tst_res(TINFO, "Test begin time: %ld.%lds", tv.tv_sec, tv.tv_usec);
	}
	pid = fork();
	if (pid == 0) {
		tst_no_corefile(0);
		set_setitimer_value(time_sec, time_usec);
		TST_EXP_PASS(sys_setitimer(tc->which, value, NULL));
		set_setitimer_value(5 * time_sec, 7 * time_usec);
		TST_EXP_PASS(sys_setitimer(tc->which, value, ovalue));
		TST_EXP_EQ_LI(ovalue->it_interval.tv_sec, time_sec);
		TST_EXP_EQ_LI(ovalue->it_interval.tv_usec, time_usec);
		tst_res(TINFO, "ovalue->it_value.tv_sec=%ld, ovalue->it_value.tv_usec=%ld",
			ovalue->it_value.tv_sec, ovalue->it_value.tv_usec);
		 * ITIMER_VIRTUAL and ITIMER_PROF timers always expire a
		 * time_step afterward the elapsed time to make sure that
		 * at least counters take effect.
		 */
		margin = tc->which == ITIMER_REAL ? 0 : time_step;
		if (ovalue->it_value.tv_sec == time_sec) {
			if (ovalue->it_value.tv_usec < 0 ||
					ovalue->it_value.tv_usec > time_usec + margin)
				tst_res(TFAIL, "ovalue->it_value.tv_usec is out of range: %ld",
					ovalue->it_value.tv_usec);
		} else {
			if (ovalue->it_value.tv_sec < 0 ||
					ovalue->it_value.tv_sec > time_sec)
				tst_res(TFAIL, "ovalue->it_value.tv_sec is out of range: %ld",
					ovalue->it_value.tv_sec);
		}
		signal(tc->signo, sig_routine);
		set_setitimer_value(0, time_usec);
		TST_EXP_PASS(sys_setitimer(tc->which, value, NULL));
		while (sigcnt <= 10UL)
			;
		signal(tc->signo, SIG_DFL);
		while (1)
			;
	}
	waitpid(pid, &status, 0);
	if (WIFSIGNALED(status) && WTERMSIG(status) == tc->signo)
		tst_res(TPASS, "Child received signal: %s", tst_strsig(tc->signo));
	else
		tst_res(TFAIL, "Child: %s", tst_strstatus(status));
	if (tc->which == ITIMER_REAL) {
		if (gettimeofday(&tv, NULL) == -1)
			tst_brk(TBROK | TERRNO, "gettimeofday(&tv1, NULL) failed");
		else
			tst_res(TINFO, "Test end time: %ld.%lds", tv.tv_sec, tv.tv_usec);
	}
}

void setup(void)
{
	struct timespec time_res;
	 * Use CLOCK_MONOTONIC_COARSE resolution for all timers, since its value is
	 * bigger than CLOCK_MONOTONIC and therefore can used for both realtime and
	 * virtual/prof timers resolutions.
	 */
	clock_getres(CLOCK_MONOTONIC_COARSE, &time_res);
	time_step = time_res.tv_nsec / 1000;
	if (time_step <= 0)
		time_step = 1000;
	tst_res(TINFO, "clock low-resolution: %luns, time step: %luus",
		time_res.tv_nsec, time_step);
	time_sec  = 9 + time_step / 1000;
	time_usec = 3 * time_step;
}

void main(){
	setup();
	cleanup();
}
