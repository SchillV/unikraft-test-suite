#include "incl.h"

int  verify_alarm(
{
	pid_t pid;
	TST_EXP_PASS_SILENT(alarm(100));
	pid = fork();
	if (pid == 0) {
		TST_EXP_PASS(alarm(0), "alarm(0) in child process");
		exit(0);
	}
	TST_EXP_VAL(alarm(0), 100, "alarm(0) in parent process");
}

void main(){
	setup();
	cleanup();
}
