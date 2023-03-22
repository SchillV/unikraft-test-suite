#include "incl.h"

volatile int alarms_received;

void sigproc(int sig)
{
	if (sig == SIGALRM)
		alarms_received++;
}

void setup(void)
{
	signal(SIGALRM, sigproc);
}

int  verify_alarm(
{
	TST_EXP_PASS_SILENT(alarm(2));
	sleep(1);
	TST_EXP_VAL(alarm(0), 1);
	sleep(2);
	TST_EXP_EQ_LU(alarms_received, 0);
}

void main(){
	setup();
	cleanup();
}
