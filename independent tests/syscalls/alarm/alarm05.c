#include "incl.h"

volatile int alarms_fired;

void run(void)
{
	alarms_fired = 0;
	TST_EXP_PASS(alarm(10));
	sleep(1);
	TST_EXP_VAL(alarm(1), 9);
	sleep(2);
	TST_EXP_EQ_LU(alarms_fired, 1);
}

void sighandler(int sig)
{
	if (sig == SIGALRM)
		alarms_fired++;
}

void setup(void)
{
	signal(SIGALRM, sighandler);
}

