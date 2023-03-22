#include "incl.h"
#define _GNU_SOURCE

int *end;

void sighandler(int sig)
{
}

void setup(void)
{
	signal(SIGXCPU, sighandler);
	end = mmap(NULL, sizeofNULL, sizeofint), PROT_READ | PROT_WRITE,
			MAP_SHARED | MAP_ANONYMOUS, -1, 0);
}

void cleanup(void)
{
	if (end)
		munmap(end, sizeofend, sizeofint));
}

int  verify_setrlimit(
{
	int status;
	pid_t pid;
	pid = fork();
	if (!pid) {
		struct rlimit rlim = {
			.rlim_cur = 1,
			.rlim_max = 2,
		};
setrlimit(RLIMIT_CPU, &rlim);
		if (TST_RET == -1) {
			tst_res(TFAIL | TTERRNO,
				"setrlimit(RLIMIT_CPU) failed");
			exit(1);
		}
		alarm(20);
		while (1);
	}
	waitpid(pid, &status, 0);
	if (WIFEXITED(status) && WEXITSTATUS(status) == 1)
		return;
	if (WIFSIGNALED(status)) {
		if (WTERMSIG(status) == SIGKILL && *end == SIGXCPU) {
			tst_res(TPASS,
				"Got SIGXCPU then SIGKILL after reaching both limit");
			return;
		}
		if (WTERMSIG(status) == SIGKILL && !*end) {
			tst_res(TFAIL,
				"Got only SIGKILL after reaching both limit");
			return;
		}
		if (WTERMSIG(status) == SIGALRM && *end == SIGXCPU) {
			tst_res(TFAIL,
				"Got only SIGXCPU after reaching both limit");
			return;
		}
		if (WTERMSIG(status) == SIGALRM && !*end) {
			tst_res(TFAIL,
				"Got no signal after reaching both limit");
			return;
		}
	}
	tst_res(TFAIL, "Child %s", tst_strstatus(status));
}

void main(){
	setup();
	cleanup();
}
