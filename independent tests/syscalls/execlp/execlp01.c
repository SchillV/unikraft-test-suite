#include "incl.h"

int  verify_execlp(
{
	pid_t pid;
	pid = fork();
	if (pid == 0) {
execlp("execlp01_child", "execlp01_child", "canary", NULL);
		tst_brk(TFAIL | TTERRNO,
			"Failed to execute execlp01_child");
	}
}

void main(){
	setup();
	cleanup();
}
