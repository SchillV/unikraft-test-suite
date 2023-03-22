#include "incl.h"
#ifdef __x86_64__
#ifndef PTRACE_GETREGSET
# define PTRACE_GETREGSET 0x4204
#endif
#ifndef PTRACE_SETREGSET
# define PTRACE_SETREGSET 0x4205
#endif
#ifndef NT_X86_XSTATE
# define NT_X86_XSTATE 0x202
#endif
#ifndef CPUID_LEAF_XSTATE
# define CPUID_LEAF_XSTATE 0xd
#endif

void check_regs_loop(uint32_t initval)
{
	const unsigned long num_iters = 1000000000;
	uint32_t xmm0[4] = { initval, initval, initval, initval };
	int status = 1;
	asm volatile("   movdqu %0, %%xmm0\n"
		     "   mov %0, %%rbx\n"
		     "1: dec %2\n"
		     "   jz 2f\n"
		     "   movdqu %%xmm0, %0\n"
		     "   mov %0, %%rax\n"
		     "   cmp %%rax, %%rbx\n"
		     "   je 1b\n"
		     "   jmp 3f\n"
		     "2: mov $0, %1\n"
		     "3:\n"
		     : "+m" (xmm0), "+r" (status)
		     : "r" (num_iters) : "rax", "rbx", "xmm0");
	if (status) {
		tst_res(TFAIL,
			"xmm registers corrupted!  initval=%08X, xmm0=%08X%08X%08X%08X\n",
			initval, xmm0[0], xmm0[1], xmm0[2], xmm0[3]);
	}
	exit(status);
}

void do_test(void)
{
	int i;
	int num_cpus = tst_ncpus();
	pid_t pid;
	uint32_t eax = 0, ebx = 0, ecx = 0, edx = 0;
	uint64_t *xstate;
	 * CPUID.(EAX=0DH, ECX=0H):EBX: maximum size (bytes, from the beginning
	 * of the XSAVE/XRSTOR save area) required by enabled features in XCR0.
	 */
	__cpuid_count(CPUID_LEAF_XSTATE, ecx, eax, ebx, ecx, edx);
	xstate = memalign(64, ebx);
	struct iovec iov = { .iov_base = xstate, .iov_len = ebx };
	int status;
	bool okay;
	tst_res(TINFO, "CPUID.(EAX=%u, ECX=0):EAX=%u, EBX=%u, ECX=%u, EDX=%u",
		CPUID_LEAF_XSTATE, eax, ebx, ecx, edx);
	pid = fork();
	if (pid == 0) {
		TST_CHECKPOINT_WAKE(0);
		check_regs_loop(0x00000000);
	}
	for (i = 0; i < num_cpus; i++) {
		if (fork() == 0)
			check_regs_loop(0xDEADBEEF);
	}
	TST_CHECKPOINT_WAIT(0);
	sched_yield();
ptrace(PTRACE_ATTACH, pid, 0, 0);
	if (TST_RET != 0) {
		free(xstate);
		tst_brk(TBROK | TTERRNO, "PTRACE_ATTACH failed");
	}
	waitpid(pid, NULL, 0);
ptrace(PTRACE_GETREGSET, pid, NT_X86_XSTATE, &iov);
	if (TST_RET != 0) {
		free(xstate);
		if (TST_ERR == EIO)
			tst_brk(TCONF, "GETREGSET/SETREGSET is unsupported");
		if (TST_ERR == EINVAL)
			tst_brk(TCONF, "NT_X86_XSTATE is unsupported");
		if (TST_ERR == ENODEV)
			tst_brk(TCONF, "CPU doesn't support XSAVE instruction");
		tst_brk(TBROK | TTERRNO,
			"PTRACE_GETREGSET failed with unexpected error");
	}
	 * Old kernels simply masked out all the reserved bits in the xstate
	 * header (causing the PTRACE_SETREGSET command here to succeed), while
	 * new kernels will reject them (causing the PTRACE_SETREGSET command
	 * here to fail with EINVAL).  We accept either behavior, as neither
	 * behavior reliably tells us whether the real bug (which we test for
	 * below in either case) is present.
	 */
ptrace(PTRACE_SETREGSET, pid, NT_X86_XSTATE, &iov);
	if (TST_RET == 0) {
		tst_res(TINFO, "PTRACE_SETREGSET with reserved bits succeeded");
	} else if (TST_ERR == EINVAL) {
		tst_res(TINFO,
			"PTRACE_SETREGSET with reserved bits failed with EINVAL");
	} else {
		free(xstate);
		tst_brk(TBROK | TTERRNO,
			"PTRACE_SETREGSET failed with unexpected error");
	}
	 * It is possible for test child 'pid' to crash on AMD
	 * systems (e.g. AMD Opteron(TM) Processor 6234) with
	 * older kernels. This causes tracee to stop and sleep
	 * in ptrace_stop(). Without resuming the tracee, the
	 * test hangs at do_test()->tst_reap_children() called
	 * by the library. Use detach here, so we don't need to
	 * worry about potential stops after this point.
	 */
ptrace(PTRACE_DETACH, pid, 0, 0);
	if (TST_RET != 0) {
		free(xstate);
		tst_brk(TBROK | TTERRNO, "PTRACE_DETACH failed");
	}
	waitpid(pid, &status, 0);
	if (WIFEXITED(status)) {
		tst_res(TINFO, "test child %d exited, retcode: %d",
			pid, WEXITSTATUS(status));
	}
	if (WIFSIGNALED(status)) {
		tst_res(TINFO, "test child %d exited, termsig: %d",
			pid, WTERMSIG(status));
	}
	okay = true;
	for (i = 0; i < num_cpus; i++) {
		wait(&status);
		okay &= (WIFEXITED(status) && WEXITSTATUS(status) == 0);
	}
	if (okay)
		tst_res(TPASS, "wasn't able to set invalid FPU state");
	free(xstate);
}

void main(){
	setup();
	cleanup();
}
