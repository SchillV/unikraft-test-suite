#include "incl.h"
#define _GNU_SOURCE
#define STACK_LIMIT (512 * 1024)

void test_setrlimit(void)
{
	int status;
	struct rlimit rlim;
	pid_t child;
	rlim.rlim_cur = STACK_LIMIT;
	rlim.rlim_max = STACK_LIMIT;
	setrlimit(RLIMIT_STACK, &rlim);
	child = fork();
	if (child == 0)
		execlp("/bin/true", "/bin/true", NULL);
	waitpid(child, &status, 0);
	if (WIFEXITED(status) && WEXITSTATUS(status) == 0) {
		tst_res(TPASS, "child process completed OK");
		return;
	}
	tst_res(TFAIL, "child %s", tst_strstatus(status));
}

void main(){
	setup();
	cleanup();
}
