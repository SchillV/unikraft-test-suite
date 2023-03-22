#include "incl.h"

int  verify_execv(
{
	pid_t pid;
	char *const args[] = { "execv01_child", "canary", NULL};
	char path[512];
	if (tst_get_path("execv01_child", path, sizeof(path)))
		tst_brk(TCONF, "Couldn't find execv01_child in $PATH");
	pid = fork();
	if (pid == 0) {
execv(path, args);
		tst_brk(TFAIL | TTERRNO, "Failed to execute execv01_child");
	}
}

void main(){
	setup();
	cleanup();
}
