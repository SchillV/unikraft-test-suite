#include "incl.h"

int  verify_execvp(
{
	pid_t pid;
	char *const args[] = {"execvp01_child", "canary", NULL};
	pid = fork();
	if (pid == 0) {
		execvp("execvp01_child", args);
		tst_brk(TFAIL | TERRNO,
			"Failed to execute execvp01_child");
	}
}

void main(){
	setup();
	cleanup();
}
