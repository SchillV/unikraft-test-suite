#include "incl.h"

short watchpoint;

pid_t child_pid;

int child_main(void)
{
	ptrace(PTRACE_TRACEME, 0, NULL, NULL);
	raise(SIGSTOP);
	asm volatile(
		"mov %%ss, %0\n"
		"mov %0, %%ss\n"
		"int $3\n"
		: "+m" (watchpoint)
	);
	return 0;
}

void run(void)
{
	int status;
#if defined(__i386__) || defined(__x86_64__)
	child_pid = fork();
	if (!child_pid) {
		exit(child_main());
	}
	if (waitpid(child_pid, &status, 0) != child_pid)
		tst_brk(TBROK, "Received event from unexpected PID");
	ptrace(PTRACE_POKEUSER, child_pid,
		(void *)offsetof(struct user, u_debugreg[0]), &watchpoint);
	ptrace(PTRACE_POKEUSER, child_pid,
		(void *)offsetof(struct user, u_debugreg[7]), (void *)0x30001);
	ptrace(PTRACE_CONT, child_pid, NULL, NULL);
#endif
	while (1) {
		if (waitpid(child_pid, &status, 0) != child_pid)
			tst_brk(TBROK, "Received event from unexpected PID");
		if (WIFEXITED(status)) {
			child_pid = 0;
			break;
		}
		if (WIFSTOPPED(status)) {
			ptrace(PTRACE_CONT, child_pid, NULL, NULL);
			continue;
		}
		tst_brk(TBROK, "Unexpected event from child");
	}
	tst_res(TPASS, "We're still here. Nothing bad happened, probably.");
}

void cleanup(void)
{
	if (child_pid)
		kill(child_pid, SIGKILL);
}

