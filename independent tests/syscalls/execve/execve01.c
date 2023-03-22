#include "incl.h"

void verify_execve()
{
	pid_t pid;
	char *const args[] = {"execve01_child", "canary", NULL};
	char path[512]="./execve01_child";
	char ipc_env_var[1024];
	char *const envp[] = { "LTP_TEST_ENV_VAR=test", ipc_env_var, NULL };
	pid = fork();
	if (pid == 0) {
		execve(path, args, envp);
		printf("Failed to execute execl01_child, error number %d\n", errno);
	}
}

void main(){
	verify_execve();
}
