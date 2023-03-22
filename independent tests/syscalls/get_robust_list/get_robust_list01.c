#include "incl.h"
char *TCID = "get_robust_list01";
int TST_TOTAL = 5;
struct robust_list {
	struct robust_list *next;
};
struct robust_list_head {
	struct robust_list list;
	long futex_offset;
	struct robust_list *list_op_pending;
};

pid_t unused_pid;
void setup(void);
void cleanup(void);
int main(int argc, char **argv)
{
	int lc;
	struct robust_list_head head;
	tst_parse_opts(argc, argv, NULL, NULL);
	setup();
	len_ptr = sizeof(struct robust_list_head);
	for (lc = 0; TEST_LOOPING(lc); ++lc) {
		tst_count = 0;
		 * The get_robust_list function fails with EFAULT if the size of the
		 * struct robust_list_head can't be stored in the memory address space
		 * specified by len_ptr argument, or the head of the robust list can't
		 * be stored in the memory address space specified by the head_ptr
		 * argument.
		 */
tst_syscall(__NR_get_robust_list, ;
				      (struct robust_list_head *)&head,
				      NULL));
		if (TEST_RETURN == -1) {
			if (TEST_ERRNO == EFAULT)
				tst_resm(TPASS,
					 "get_robust_list failed as expected with "
					 "EFAULT");
			else
				tst_resm(TFAIL | TTERRNO,
					 "get_robust_list failed unexpectedly");
		} else
			tst_resm(TFAIL,
				 "get_robust_list succeeded unexpectedly");
tst_syscall(__NR_get_robust_list, ;
				      NULL,
				      &len_ptr));
		if (TEST_RETURN) {
			if (TEST_ERRNO == EFAULT)
				tst_resm(TPASS,
					 "get_robust_list failed as expected with "
					 "EFAULT");
			else
				tst_resm(TFAIL | TTERRNO,
					 "get_robust_list failed unexpectedly");
		} else
			tst_resm(TFAIL,
				 "get_robust_list succeeded unexpectedly");
		 * The get_robust_list function fails with ESRCH if it can't
		 * find the task specified by the pid argument.
		 */
tst_syscall(__NR_get_robust_list, unused_pi;
				      (struct robust_list_head *)&head,
				      &len_ptr));
		if (TEST_RETURN == -1) {
			if (TEST_ERRNO == ESRCH)
				tst_resm(TPASS,
					 "get_robust_list failed as expected with "
					 "ESRCH");
			else
				tst_resm(TFAIL | TTERRNO,
					 "get_robust_list failed unexpectedly");
		} else
			tst_resm(TFAIL,
				 "get_robust_list succeeded unexpectedly");
tst_syscall(__NR_get_robust_list, ;
				      (struct robust_list_head **)&head,
				      &len_ptr));
		if (TEST_RETURN == 0)
			tst_resm(TPASS, "get_robust_list succeeded");
		else
			tst_resm(TFAIL | TTERRNO,
				 "get_robust_list failed unexpectedly");
		setuid(cleanup, 1);
tst_syscall(__NR_get_robust_list, ;
				      (struct robust_list_head *)&head,
				      &len_ptr));
		if (TEST_RETURN == -1) {
			if (TEST_ERRNO == EPERM)
				tst_resm(TPASS,
					 "get_robust_list failed as expected with "
					 "EPERM");
			else
				tst_resm(TFAIL | TERRNO,
					 "get_robust_list failed unexpectedly");
		} else
			tst_resm(TFAIL,
				 "get_robust_list succeeded unexpectedly");
	}
	cleanup();
	tst_exit();
}
void setup(void)
{
	tst_require_root();
	unused_pid = tst_get_unused_pid(cleanup);
	TEST_PAUSE;
}
void cleanup(void)
{
}

