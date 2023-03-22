#include "incl.h"
char *TCID = "fork05";

char *environ_list[] = { "TERM", "NoTSetzWq", "TESTPROG" };
#define NUMBER_OF_ENVIRON (sizeof(environ_list)/sizeof(char *))
int TST_TOTAL = NUMBER_OF_ENVIRON;
#if defined(linux) && defined(__i386__)
struct modify_ldt_ldt_s {
	unsigned int entry_number;
	unsigned long int base_addr;
	unsigned int limit;
	unsigned int seg_32bit:1;
	unsigned int contents:2;
	unsigned int read_exec_only:1;
	unsigned int limit_in_pages:1;
	unsigned int seg_not_present:1;
	unsigned int useable:1;
	unsigned int empty:25;
};

int a = 42;

void modify_ldt(int func, struct modify_ldt_ldt_s *ptr, int bytecount)
{
	tst_syscall(__NR_modify_ldt, func, ptr, bytecount);
}
int main(void)
{
	struct modify_ldt_ldt_s ldt0;
	int lo;
	pid_t pid;
	int res;
	ldt0.entry_number = 0;
	ldt0.base_addr = (long)&a;
	ldt0.limit = 4;
	ldt0.seg_32bit = 1;
	ldt0.contents = 0;
	ldt0.read_exec_only = 0;
	ldt0.limit_in_pages = 0;
	ldt0.seg_not_present = 0;
	ldt0.useable = 1;
	ldt0.empty = 0;
	modify_ldt(1, &ldt0, sizeof(ldt0));
	asm volatile ("movw %w0, %%fs"::"q" (7));
	asm volatile ("movl %%fs:0, %0":"=r" (lo));
	tst_resm(TINFO, "a = %d", lo);
	asm volatile ("pushl %%fs; popl %0":"=q" (lo));
	tst_resm(TINFO, "%%fs = %#06hx", lo);
	asm volatile ("movl %0, %%fs:0"::"r" (99));
	pid = fork();
	if (pid == 0) {
		asm volatile ("pushl %%fs; popl %0":"=q" (lo));
		tst_resm(TINFO, "%%fs = %#06hx", lo);
		asm volatile ("movl %%fs:0, %0":"=r" (lo));
		tst_resm(TINFO, "a = %d", lo);
		if (lo != 99)
			tst_resm(TFAIL, "Test failed");
		else
			tst_resm(TPASS, "Test passed");
		exit(lo != 99);
	} else {
		waitpid(pid, &res, 0);
	}
	return WIFSIGNALED(res);
}
int main(void)
{
	tst_resm(TINFO, "%%fs test only for ix86");
	 * should be successful on all non-ix86 platforms.
	 */
	tst_exit();
}

