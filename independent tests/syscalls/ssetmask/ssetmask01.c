#include "incl.h"
char *TCID = "ssetmask01";
int testno;
int TST_TOTAL = 2;
void cleanup(void)
{
	tst_rmdir();
}
void setup(void)
{
	TEST_PAUSE;
	tst_tmpdir();
}
int main(int ac, char **av)
{
	int lc;
	tst_parse_opts(ac, av, NULL, NULL);
	setup();
	for (lc = 0; TEST_LOOPING(lc); ++lc) {
		tst_count = 0;
		for (testno = 0; testno < TST_TOTAL; ++testno) {
			tst_syscall(__NR_ssetmask, SIGALRM);
tst_syscall(__NR_sgetmask);
			if (TEST_RETURN != SIGALRM) {
				tst_brkm(TFAIL | TTERRNO, cleanup,
					 "sgetmask() failed");
			}
tst_syscall(__NR_ssetmask, SIGUSR1);
			if (TEST_RETURN != SIGALRM) {
				tst_brkm(TFAIL | TTERRNO, cleanup,
					 "ssetmask() failed");
			}
			tst_resm(TPASS, "Got SIGALRM--Test PASS ");
		}
	}
	cleanup();
	tst_exit();
}

