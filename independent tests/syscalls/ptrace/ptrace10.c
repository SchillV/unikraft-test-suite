#include "incl.h"

pid_t child_pid;

void child_main(void)
{
	raise(SIGSTOP);
	exit(0);
}

void run(void)
{
	int status;
	unsigned long addr;
	child_pid = fork();
	if (!child_pid)
		child_main();
	if (waitpid(child_pid, &status, WUNTRACED) != child_pid)
		tst_brk(TBROK, "Received event from unexpected PID");
#if defined(__i386__) || defined(__x86_64__)
	ptrace(PTRACE_ATTACH, child_pid, NULL, NULL);
	ptrace(PTRACE_POKEUSER, child_pid,
		(void *)offsetof(struct user, u_debugreg[0]), (void *)1);
	ptrace(PTRACE_POKEUSER, child_pid,
		(void *)offsetof(struct user, u_debugreg[0]), (void *)2);
	addr = ptrace(PTRACE_PEEKUSER, child_pid,
	              (void*)offsetof(struct user, u_debugreg[0]), NULL);
#endif
	if (addr == 2)
		tst_res(TPASS, "The rd0 was set on second PTRACE_POKEUSR");
	else
		tst_res(TFAIL, "The rd0 wasn't set on second PTRACE_POKEUSER");
	ptrace(PTRACE_DETACH, child_pid, NULL, NULL);
	kill(child_pid, SIGCONT);
	child_pid = 0;
	tst_reap_children();
}

void cleanup(void)
{
	if (child_pid)
		kill(child_pid, SIGKILL);
}

