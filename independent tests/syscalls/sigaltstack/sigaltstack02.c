#include "incl.h"
#define INVAL_FLAGS	9999
char *TCID = "sigaltstack02";
int TST_TOTAL = 2;
	int flag;
	int size;
	char *desc;
	int exp_errno;
} Test_cases[] = {
	{
	INVAL_FLAGS, SIGSTKSZ, "Invalid Flag value", EINVAL},
	 * avoid using MINSIGSTKSZ defined by glibc as it could be different
	 * from the one in kernel ABI
	 */
	{
	0, (2048 - 1), "alternate stack is < MINSIGSTKSZ", ENOMEM},
	{
	0, 0, NULL, 0}
};
int main(int ac, char **av)
{
	int lc;
	tst_parse_opts(ac, av, NULL, NULL);
	setup();
	for (lc = 0; TEST_LOOPING(lc); lc++) {
		tst_count = 0;
		for (ind = 0; Test_cases[ind].desc != NULL; ind++) {
			sigstk.ss_size = Test_cases[ind].size;
			sigstk.ss_flags = Test_cases[ind].flag;
			test_desc = Test_cases[ind].desc;
sigaltstack(&sigstk, NULL);
			if (TEST_RETURN == -1) {
				if (TEST_ERRNO ==
				    Test_cases[ind].exp_errno) {
					tst_resm(TPASS, "stgaltstack() "
						 "fails, %s, errno:%d",
						 test_desc, TEST_ERRNO);
				} else {
					tst_resm(TFAIL, "sigaltstack() "
						 "fails, %s, errno:%d, "
						 "expected errno:%d",
						 test_desc, TEST_ERRNO,
						 Test_cases
						 [ind].exp_errno);
				}
			} else {
				tst_resm(TFAIL, "sigaltstack() returned %ld, "
					 "expected -1, errno:%d", TEST_RETURN,
					 Test_cases[ind].exp_errno);
			}
		}
	}
	cleanup();
	tst_exit();
}
void setup(void)
{
	tst_sig(FORK, DEF_HANDLER, cleanup);
	TEST_PAUSE;
	if ((sigstk.ss_sp = malloc(SIGSTKSZ)) == NULL) {
		tst_brkm(TFAIL, cleanup,
			 "could not allocate memory for the alternate stack");
	}
}
void cleanup(void)
{
	free(sigstk.ss_sp);
}

