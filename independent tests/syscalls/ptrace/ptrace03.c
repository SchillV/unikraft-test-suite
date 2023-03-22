#include "incl.h"

pid_t unused_pid;

pid_t zero_pid;

struct tcase {
	int request;
	pid_t *pid;
	int exp_errno;
	char *tname;
} tcases[] = {
	{PTRACE_ATTACH, &unused_pid, ESRCH,
	"Trace a process which does not exist"},
	{PTRACE_TRACEME, &zero_pid, EPERM,
	"Trace a process which is already been traced"}
};

int  verify_ptrace(unsigned int n)
{
	struct tcase *tc = &tcases[n];
	int child_pid;
	tst_res(TINFO, "%s", tc->tname);
	child_pid = fork();
	if (!child_pid) {
		if (tc->exp_errno == EPERM)
			ptrace(PTRACE_TRACEME, 0, NULL, NULL);
ptrace(tc->request, *(tc->pid), NULL, NULL);
		if (TST_RET == 0) {
			tst_res(TFAIL, "ptrace() succeeded unexpectedly");
			exit(1);
		}
		if (tc->exp_errno == TST_ERR)
			tst_res(TPASS | TTERRNO, "ptrace() failed as expected");
		else
			tst_res(TFAIL | TTERRNO, "ptrace() failed unexpectedly, expected %s",
					tst_strerrno(tc->exp_errno));
	}
	tst_reap_children();
}

void setup(void)
{
	unused_pid = tst_get_unused_pid();
}

void main(){
	setup();
	cleanup();
}
