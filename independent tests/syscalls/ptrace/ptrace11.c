#include "incl.h"

int  verify_ptrace(
{
ptrace(PTRACE_ATTACH, 1, NULL, NULL);
	if (TST_RET == 0)
		tst_res(TPASS, "ptrace() traces init process successfully");
	else
		tst_res(TFAIL | TTERRNO,
			"ptrace() returns %ld, failed unexpectedly", TST_RET);
	 * Wait until tracee is stopped by SIGSTOP otherwise detach will fail
	 * with ESRCH.
	 */
	waitpid(1, NULL, 0);
	ptrace(PTRACE_DETACH, 1, NULL, NULL);
}

void main(){
	setup();
	cleanup();
}
