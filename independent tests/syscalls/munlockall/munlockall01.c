#include "incl.h"
void setup();
void cleanup();
char *TCID = "munlockall01";
int TST_TOTAL = 1;
#if !defined(UCLINUX)
int main(int ac, char **av)
{
	int lc;
	tst_parse_opts(ac, av, NULL, NULL);
	setup();
	for (lc = 0; TEST_LOOPING(lc); lc++) {
		tst_count = 0;
munlockall();
		if (TEST_RETURN == -1) {
			tst_resm(TFAIL | TTERRNO, "munlockall() Failed with"
				 " return=%ld", TEST_RETURN);
		} else {
			tst_resm(TPASS, "munlockall() passed with"
				 " return=%ld ", TEST_RETURN);
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
void setup(void)
{
	tst_require_root();
	tst_sig(NOFORK, DEF_HANDLER, cleanup);
	TEST_PAUSE;
}
void cleanup(void)
{
}

