#include "incl.h"
TCID_DEFINE(modify_ldt02);
int TST_TOTAL = 1;
#if defined(__i386__) && defined(HAVE_MODIFY_LDT)
#ifdef HAVE_ASM_LDT_H
#endif
extern int modify_ldt(int, void *, unsigned long);
#ifdef HAVE_STRUCT_USER_DESC
typedef struct user_desc modify_ldt_s;
#elif  HAVE_STRUCT_MODIFY_LDT_LDT_S
typedef struct modify_ldt_ldt_s modify_ldt_s;
#else
typedef struct modify_ldt_ldt_t {
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
} modify_ldt_s;
#endif
int create_segment(void *, size_t);
int read_segment(unsigned int);
void cleanup(void);
void setup(void);
int main(int ac, char **av)
{
	int lc;
	int val, pid, status;
	int seg[4];
	tst_parse_opts(ac, av, NULL, NULL);
	setup();
	for (lc = 0; TEST_LOOPING(lc); lc++) {
		tst_count = 0;
		seg[0] = 12345;
		if (create_segment(seg, sizeof(seg)) == -1) {
			tst_brkm(TBROK, cleanup, "Creation of segment failed");
		}
		val = read_segment(0);
		if (val != seg[0]) {
			tst_resm(TFAIL, "Invalid value read %d, expected %d",
				 val, seg[0]);
		} else
			tst_resm(TPASS, "value read as expected");
		if (create_segment(0, 10) == -1) {
			tst_brkm(TBROK, cleanup, "Creation of segment failed");
		}
		tst_old_flush();
		if ((pid = FORK_OR_VFORK()) == 0) {
			signal(SIGSEGV, SIG_DFL);
			val = read_segment(0);
			exit(1);
		}
		(void)waitpid(pid, &status, 0);
		if (WEXITSTATUS(status) != 0) {
			tst_resm(TFAIL, "Did not generate SEGV, child returned "
				 "unexpected status");
		} else {
			if (WIFSIGNALED(status) && (WTERMSIG(status) == SIGSEGV))
				tst_resm(TPASS, "generate SEGV as expected");
			else
				tst_resm(TFAIL, "Did not generate SEGV");
		}
	}
	cleanup();
	tst_exit();
}
int create_segment(void *seg, size_t size)
{
	modify_ldt_s entry;
	entry.entry_number = 0;
	entry.base_addr = (unsigned long)seg;
	entry.limit = size;
	entry.seg_32bit = 1;
	entry.contents = 0;
	entry.read_exec_only = 0;
	entry.limit_in_pages = 0;
	entry.seg_not_present = 0;
	return modify_ldt(1, &entry, sizeof(entry));
}
int read_segment(unsigned int index)
{
	int res;
	__asm__ __volatile__("\n\
			push    $0x0007;\n\
			pop     %%fs;\n\
			movl    %%fs:(%1), %0":"=r"(res)
			     :"r"(index * sizeof(int)));
	return res;
}
void setup(void)
{
	tst_sig(FORK, DEF_HANDLER, cleanup);
	TEST_PAUSE;
}
void cleanup(void)
{
}
#elif HAVE_MODIFY_LDT
int main(void)
{
	tst_brkm(TCONF,
		 NULL,
		 "modify_ldt is available but not tested on the platform than __i386__");
}
int main(void)
{
	tst_resm(TINFO, "modify_ldt02 test only for ix86");
	tst_exit();
}

