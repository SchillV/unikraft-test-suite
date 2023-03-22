#include "incl.h"
char *TCID = "gettimeofday01";
int TST_TOTAL = 1;
#if !defined UCLINUX
void cleanup(void);
void setup(void);
int main(int ac, char **av)
{
	int lc;
	int ret;
	tst_parse_opts(ac, av, NULL, NULL);
	setup();
	for (lc = 0; TEST_LOOPING(lc); lc++) {
		tst_count = 0;
tst_syscall(__NR_gettimeofday, (void *)-1, (void *)-1);
		 * TEST_RETURN into an int to test with */
		ret = TEST_RETURN;
		if (ret != -1) {
			tst_resm(TFAIL,
				 "call succeeded unexpectedly (got back %i, wanted -1)",
				 ret);
			continue;
		}
		if (TEST_ERRNO == EFAULT)
			tst_resm(TPASS,
				 "gettimeofday(2) set the errno EFAULT correctly");
		else
			tst_resm(TFAIL,
				 "gettimeofday(2) didn't set errno to EFAULT, errno=%i (%s)",
				 errno, strerror(errno));
	}
	cleanup();
	tst_exit();
}
void setup(void)
{
	tst_sig(NOFORK, DEF_HANDLER, cleanup);
	TEST_PAUSE;
}
void cleanup(void)
{
}
#else
int main(void)
{
	tst_brkm(TCONF, "gettimeofday EFAULT check disabled on uClinux");
}
#endif

