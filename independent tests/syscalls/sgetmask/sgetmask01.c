#include "incl.h"
# Sometime the returned "Oops"in this case don't mean anything for
# correct or error, we check the result between different kernel and
# try to find if there exist different returned code in different kernel
#
*/
char *TCID = "sgetmask01";
int testno;
int TST_TOTAL = 2;
void cleanup(void)
{
	tst_rmdir();
	tst_exit();
}
void setup(void)
{
	TEST_PAUSE;
	tst_tmpdir();
}
int main(int ac, char **av)
{
	int sig;
	int lc;
	tst_parse_opts(ac, av, NULL, NULL);
	setup();
	for (lc = 0; TEST_LOOPING(lc); ++lc) {
		tst_count = 0;
		for (testno = 0; testno < TST_TOTAL; ++testno) {
			for (sig = -3; sig <= SIGRTMAX + 1; sig++) {
tst_syscall(__NR_ssetmask, sig);
				tst_resm(TINFO, "Setting signal : %d -- "
					"return of setmask : %ld",
					sig, TEST_RETURN);
tst_syscall(__NR_sgetmask);
				if (TEST_RETURN != sig) {
					tst_resm(TINFO,
						 "Oops,setting sig %d, got %ld",
						 sig, TEST_RETURN);
				} else
					tst_resm(TPASS,
						 "OK,setting sig %d, got %ld",
						 sig, TEST_RETURN);
				if (sig == SIGRTMAX + 1) {
					cleanup();
					tst_exit();
				}
			}
		}
	}
	cleanup();
	tst_exit();
}

