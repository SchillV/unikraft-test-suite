#include "incl.h"

int  verify_execle(
{
	pid_t pid;
	char path[512];
	char ipc_env_var[1024];
	sprintf(ipc_env_var, IPC_ENV_VAR "=%s", getenv(IPC_ENV_VAR));
	char *const envp[] = { "LTP_TEST_ENV_VAR=test", ipc_env_var, NULL };
	if (tst_get_path("execle01_child", path, sizeof(path)))
		tst_brk(TCONF, "Couldn't find execle01_child in $PATH");
	pid = fork();
	if (pid == 0) {
execle(path, "execle01_child", "canary", NULL, envp);
		tst_brk(TFAIL | TTERRNO,
			"Failed to execute execl01_child");
	}
}

void main(){
	setup();
	cleanup();
}
