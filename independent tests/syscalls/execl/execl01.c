#include "incl.h"

int  verify_execl(
{
	pid_t pid;
	char path[512];
	if (tst_get_path("execl01_child", path, sizeof(path)))
		tst_brk(TCONF, "Couldn't find execl01_child in $PATH");
	pid = fork();
	if (pid == 0) {
execl(path, "execl01_child", "canary", NULL);
		tst_brk(TFAIL | TTERRNO,
			"Failed to execute execl01_child");
	}
}

void main(){
	setup();
	cleanup();
}
