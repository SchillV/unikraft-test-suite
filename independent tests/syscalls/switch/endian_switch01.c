#include "incl.h"
#if defined(__powerpc64__) || defined(__powerpc__)
# ifndef PPC_FEATURE_TRUE_LE
# define PPC_FEATURE_TRUE_LE              0x00000002
# endif
# define TST_NO_DEFAULT_MAIN
void check_le_switch_supported(void)
{
	int status;
	if (fork() == 0) {
		syscall(0x1ebe);
		exit(errno);
	}
	wait(&status);
	if (WIFSIGNALED(status)) {
		int sig = WTERMSIG(status);
		tst_res(TINFO, "check exited with sig %d", sig);
	} else if (WIFEXITED(status)) {
		int rc = WEXITSTATUS(status);
		tst_res(TINFO, "check exited with %d", rc);
		if (rc == ENOSYS)
			tst_brk(TCONF, "fast endian switch (0x1ebe) N/A");
	}
}
void test_le_switch(void)
{
	int status;
	if (fork() == 0) {
		register int r0 asm("r0") = 0x1ebe;
		asm volatile ("sc; .long 0x02000044"
				: "=&r" (r0)
				: "0"(r0)
				: "cr0", "r9", "r10", "r11", "r12");
		exit(0);
	}
	wait(&status);
	if (WIFSIGNALED(status)) {
		int sig = WTERMSIG(status);
		tst_res(TFAIL, "test exited with sig %d", sig);
	} else if (WIFEXITED(status)) {
		int rc = WEXITSTATUS(status);
		if (rc != 0)
			tst_res(TFAIL, "test exited with %d", rc);
		else
			tst_res(TPASS, "endian_switch() syscall tests passed");
	}
}

void endian_test(void)
{
	check_le_switch_supported();
	test_le_switch();
}

