#include "incl.h"
#ifdef __NR_set_robust_list
#ifndef __user
#define __user
#endif
struct robust_list {
	struct robust_list __user *next;
};
struct robust_list_head {
	struct robust_list list;
	long futex_offset;
	struct robust_list __user *list_op_pending;
};
#endif
char *TCID = "set_robust_list01";
int TST_TOTAL = 2;
void setup(void);
void cleanup(void);
int main(int argc, char **argv)
{
#ifdef __NR_set_robust_list
	int lc;
#endif
#ifdef __NR_set_robust_list
	struct robust_list_head head;
	int retval;
#endif
	tst_parse_opts(argc, argv, NULL, NULL);
	setup();
#ifdef __NR_set_robust_list
	len = sizeof(struct robust_list_head);
	for (lc = 0; TEST_LOOPING(lc); ++lc) {
		tst_count = 0;
		 * The set_robust_list function fails with EINVAL if the len argument
		 * doesn't match the size of structure struct robust_list_head.
		 */
retval = syscall(__NR_set_robust_list, &head, -1);
		if (TEST_RETURN) {
			if (TEST_ERRNO == EINVAL)
				tst_resm(TPASS,
					 "set_robust_list: retval = %ld (expected %d), "
					 "errno = %d (expected %d)",
					 TEST_RETURN, -1, TEST_ERRNO, EINVAL);
			else
				tst_resm(TFAIL,
					 "set_robust_list: retval = %ld (expected %d), "
					 "errno = %d (expected %d)",
					 TEST_RETURN, -1, TEST_ERRNO, EINVAL);
		} else {
			tst_resm(TFAIL,
				 "set_robust_list: retval = %ld (expected %d), "
				 "errno = %d (expected %d)", TEST_RETURN, -1,
				 TEST_ERRNO, EINVAL);
		}
		 * This call to set_robust_list function should be sucessful.
		 */
retval = syscall(__NR_set_robust_list, &head, len);
		if (TEST_RETURN == 0) {
			tst_resm(TPASS,
				 "set_robust_list: retval = %ld (expected %d), "
				 "errno = %d (expected %d)", TEST_RETURN, 0,
				 TEST_ERRNO, 0);
		} else {
			tst_resm(TFAIL,
				 "set_robust_list: retval = %ld (expected %d), "
				 "errno = %d (expected %d)", TEST_RETURN, 0,
				 TEST_ERRNO, 0);
		}
	}
#else
	tst_resm(TCONF, "set_robust_list: system call not available.");
#endif
	cleanup();
	exit(EXIT_SUCCESS);
}
void setup(void)
{
	TEST_PAUSE;
}
void cleanup(void)
{
}

