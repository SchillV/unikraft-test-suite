#include "incl.h"
TCID_DEFINE(modify_ldt01);
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
void cleanup(void);
void setup(void);
int main(int ac, char **av)
{
	int lc;
	void *ptr;
	int retval, func;
	int seg[4];
	tst_parse_opts(ac, av, NULL, NULL);
	setup();
	for (lc = 0; TEST_LOOPING(lc); lc++) {
		tst_count = 0;
		 * Check for ENOSYS.
		 */
		ptr = malloc(10);
		func = 100;
		retval = modify_ldt(func, ptr, sizeof(ptr));
		if (retval < 0) {
			if (errno != ENOSYS) {
				tst_resm(TFAIL, "modify_ldt() set invalid "
					 "errno, expected ENOSYS, got: %d",
					 errno);
			} else {
				tst_resm(TPASS,
					"modify_ldt() set expected errno");
			}
		} else {
			tst_resm(TFAIL, "modify_ldt error: "
				 "unexpected return value %d", retval);
		}
		free(ptr);
		 * Check for EINVAL
		 */
		ptr = 0;
		retval = modify_ldt(1, ptr, sizeof(ptr));
		if (retval < 0) {
			if (errno != EINVAL) {
				tst_resm(TFAIL, "modify_ldt() set invalid "
					 "errno, expected EINVAL, got: %d",
					 errno);
			} else {
				tst_resm(TPASS,
					"modify_ldt() set expected errno");
			}
		} else {
			tst_resm(TFAIL, "modify_ldt error: "
				 "unexpected return value %d", retval);
		}
		 * Create a new LDT segment.
		 */
		if (create_segment(seg, sizeof(seg)) == -1) {
			tst_brkm(TBROK, cleanup, "Creation of segment failed");
		}
		 * Check for EFAULT
		 */
		ptr = sbrk(0);
		retval = modify_ldt(0, ptr + 0xFFF, sizeof(ptr));
		if (retval < 0) {
			if (errno != EFAULT) {
				tst_resm(TFAIL, "modify_ldt() set invalid "
					 "errno, expected EFAULT, got: %d",
					 errno);
			} else {
				tst_resm(TPASS,
					"modify_ldt() set expected errno");
			}
		} else {
			tst_resm(TFAIL, "modify_ldt error: "
				 "unexpected return value %d", retval);
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
#else
int main(void)
{
	tst_resm(TINFO, "modify_ldt01 test only for ix86");
	tst_exit();
}

