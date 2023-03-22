#include "incl.h"

volatile int alarm_cnt;

int  verify_alarm(
{
	pid_t pid;
	alarm_cnt = 0;
	TST_EXP_PASS_SILENT(alarm(1));
	pid = fork();
	sleep(3);
	if (pid == 0) {
		TST_EXP_EQ_LU(alarm_cnt, 0);
		exit(0);
	}
	TST_EXP_EQ_LU(alarm_cnt, 1);
}

void sighandler(int sig LTP_ATTRIBUTE_UNUSED)
{
	alarm_cnt++;
}

void setup(void)
{
	signal(SIGALRM, sighandler);
}

void main(){
	setup();
	cleanup();
}
