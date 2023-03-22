#include "incl.h"
#define _GNU_SOURCE

void *bad_addr;

int  verify_setrlimit(
{
	int status;
	pid_t pid;
	pid = fork();
	if (!pid) {
setrlimit(RLIMIT_NOFILE, bad_addr);
		if (TST_RET != -1) {
			tst_res(TFAIL, "setrlimit()  succeeded unexpectedly");
			exit(0);
		}
		if (TST_ERR == EFAULT) {
			tst_res(TPASS | TTERRNO,
				"setrlimit() failed as expected");
		} else {
			tst_res(TFAIL | TTERRNO,
				"setrlimit() should fail with EFAULT, got");
		}
		exit(0);
	}
	waitpid(pid, &status, 0);
	 * in some cases, it is possible to get SegFault.
	 */
	if (WIFSIGNALED(status) && WTERMSIG(status) == SIGSEGV) {
		tst_res(TPASS, "setrlimit() caused SIGSEGV");
		return;
	}
	if (WIFEXITED(status) && WEXITSTATUS(status) == 0)
		return;
	tst_res(TFAIL, "child %s", tst_strstatus(status));
}

void setup(void)
{
	bad_addr = tst_get_bad_addr(NULL);
}

void main(){
	setup();
	cleanup();
}
