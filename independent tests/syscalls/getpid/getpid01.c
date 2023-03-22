#include "incl.h"

int  verify_getpid(
{
	pid_t pid_max, pid;
	int i;
	file_scanf("/proc/sys/kernel/pid_max", "%d\n", &pid_max);
	for (i = 0; i < 100; i++) {
		pid = fork();
		if (pid == 0) {
			pid = getpid();
			if (1 < pid && pid <= pid_max)
				tst_res(TPASS, "getpid() returns %d", pid);
			else
				tst_res(TFAIL,
					"getpid() returns out of range: %d", pid);
			exit(0);
		} else {
			wait(NULL);
		}
	}
}

void main(){
	setup();
	cleanup();
}
