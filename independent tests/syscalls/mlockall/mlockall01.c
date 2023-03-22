#include "incl.h"
void setup();
void cleanup();
char *TCID = "mlockall01";
int TST_TOTAL = 3;
#if !defined(UCLINUX)
struct test_case_t {
	int flag;
	char *fdesc;
} TC[] = {
	 * Check for all possible flags of mlockall
	 */
	{
	MCL_CURRENT, "MCL_CURRENT"}, {
	MCL_FUTURE, "MCL_FUTURE"}, {
	MCL_CURRENT | MCL_FUTURE, "MCL_CURRENT|MCL_FUTURE"}
};
int main(int ac, char **av)
{
	int lc, i;
	tst_parse_opts(ac, av, NULL, NULL);
	setup();
	for (lc = 0; TEST_LOOPING(lc); lc++) {
		tst_count = 0;
		for (i = 0; i < TST_TOTAL; i++) {
mlockall(TC[i].flag);
			if (TEST_RETURN == -1) {
				tst_resm(TFAIL | TTERRNO,
					 "mlockall(%s) Failed with "
					 "return=%ld", TC[i].fdesc,
					 TEST_RETURN);
			} else {
				tst_resm(TPASS, "mlockall test passed for %s",
					 TC[i].fdesc);
			}
		}
	}
	cleanup();
	tst_exit();
}
#else
int main(void)
{
	tst_resm(TINFO, "test is not available on uClinux");
	tst_exit();
}
#endif
void setup(void)
{
	tst_require_root();
	tst_sig(NOFORK, DEF_HANDLER, cleanup);
	TEST_PAUSE;
}
void cleanup(void)
{
}

