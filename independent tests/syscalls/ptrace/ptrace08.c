#include "incl.h"

pid_t child_pid;
#if defined(__i386__)
# define KERN_ADDR_MIN 0xc0000000
# define KERN_ADDR_MAX 0xffffffff
# define KERN_ADDR_BITS 32
#else
# define KERN_ADDR_MIN 0xffff800000000000
# define KERN_ADDR_MAX 0xffffffffffffffff
# define KERN_ADDR_BITS 64
#endif

void child_main(void)
{
	raise(SIGSTOP);
	exit(0);
}

void ptrace_try_kern_addr(unsigned long kern_addr)
{
	int status;
	unsigned long addr;
	tst_res(TINFO, "Trying address 0x%lx", kern_addr);
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
		(void *)offsetof(struct user, u_debugreg[7]), (void *)1);
ptrace(PTRACE_POKEUSER, child_pi;
		(void *)offsetof(struct user, u_debugreg[0]),
		(void *)kern_addr));
	if (TST_RET == -1) {
		addr = ptrace(PTRACE_PEEKUSER, child_pid,
					  (void *)offsetof(struct user, u_debugreg[0]), NULL);
		if (addr == kern_addr) {
ptrace(PTRACE_POKEUSER, child_pi;
				(void *)offsetof(struct user, u_debugreg[7]), (void *)1));
		}
	}
	if (TST_RET != -1) {
		tst_res(TFAIL, "ptrace() breakpoint with kernel addr succeeded");
	} else {
		if (TST_ERR == EINVAL) {
			tst_res(TPASS | TTERRNO,
				"ptrace() breakpoint with kernel addr failed");
		} else {
			tst_res(TFAIL | TTERRNO,
				"ptrace() breakpoint on kernel addr should return EINVAL, got");
		}
	}
#endif
	ptrace(PTRACE_DETACH, child_pid, NULL, NULL);
	kill(child_pid, SIGCONT);
	child_pid = 0;
	tst_reap_children();
}

void run(void)
{
	ptrace_try_kern_addr(KERN_ADDR_MIN);
	ptrace_try_kern_addr(KERN_ADDR_MAX);
	ptrace_try_kern_addr(KERN_ADDR_MIN + (KERN_ADDR_MAX - KERN_ADDR_MIN)/2);
}

void cleanup(void)
{
	if (child_pid)
		kill(child_pid, SIGKILL);
}

