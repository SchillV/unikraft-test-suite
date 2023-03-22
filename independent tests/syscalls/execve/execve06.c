#include "incl.h"

int  verify_execve()
{
	pid_t pid;
	char path[512];
	char ipc_env_var[1024];
	char *const envp[] = {ipc_env_var, NULL};
	char *const argv[] = {NULL};
	pid = fork();
	if (pid == 0) {
		execve(path, argv, envp);
		printf("Failed to execute execl01_child\n");
		return 0;
	}
	return 1;
}

void main(){
	if(verify_execve())
		printf("test succeeded\n");
}
