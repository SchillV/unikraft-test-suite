#include "incl.h"
char *TCID = "timer_getoverrun01";
int TST_TOTAL = 1;

void cleanup(void)
{
	tst_rmdir();
}

void setup(void)
{
	TEST_PAUSE;
	tst_tmpdir();
}
int main(int ac, char **av)
{
	int lc;
	int timer;
	struct sigevent ev;
	tst_parse_opts(ac, av, NULL, NULL);
	setup();
	ev.sigev_value = (union sigval) 0;
	ev.sigev_signo = SIGALRM;
	ev.sigev_notify = SIGEV_SIGNAL;
tst_syscall(__NR_timer_create, CLOCK_REALTIME, &ev, &timer);
	if (TEST_RETURN != 0)
		tst_brkm(TBROK | TTERRNO, cleanup, "Failed to create timer");
	for (lc = 0; TEST_LOOPING(lc); ++lc) {
		tst_count = 0;
tst_syscall(__NR_timer_getoverrun, timer);
		if (TEST_RETURN == 0) {
			tst_resm(TPASS,
			         "timer_getoverrun(CLOCK_REALTIME) Passed");
		} else {
			tst_resm(TFAIL | TTERRNO,
			         "timer_getoverrun(CLOCK_REALTIME) Failed");
		}
tst_syscall(__NR_timer_getoverrun, -1);
		if (TEST_RETURN == -1 && TEST_ERRNO == EINVAL) {
			tst_resm(TPASS,	"timer_gettime(-1) Failed: EINVAL");
		} else {
			tst_resm(TFAIL | TTERRNO,
			         "timer_gettime(-1) = %li", TEST_RETURN);
		}
	}
	cleanup();
	tst_exit();
}

