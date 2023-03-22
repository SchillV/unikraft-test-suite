#include "incl.h"
#define _GNU_SOURCE
char *TCID = "rt_sigaction02";

int testno;
int TST_TOTAL = 1;
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

int test_flags[] =
    { SA_RESETHAND | SA_SIGINFO, SA_RESETHAND, SA_RESETHAND | SA_SIGINFO,
SA_RESETHAND | SA_SIGINFO, SA_NOMASK };
char *test_flags_list[] =
    { "SA_RESETHAND|SA_SIGINFO", "SA_RESETHAND", "SA_RESETHAND|SA_SIGINFO",
"SA_RESETHAND|SA_SIGINFO", "SA_NOMASK" };

struct test_case_t {
	int exp_errno;
	char *errdesc;
} test_cases[] = {
	{
	EFAULT, "EFAULT"}
};
int main(int ac, char **av)
{
	unsigned int flag;
	int signal;
	int lc;
	tst_parse_opts(ac, av, NULL, NULL);
	setup();
	for (lc = 0; TEST_LOOPING(lc); ++lc) {
		tst_count = 0;
		for (testno = 0; testno < TST_TOTAL; ++testno) {
			for (signal = SIGRTMIN; signal <= SIGRTMAX; signal++) {
				tst_resm(TINFO, "Signal %d", signal);
				for (flag = 0; flag < ARRAY_SIZE(test_flags); flag++) {
					 * long sys_rt_sigaction (int sig, const struct sigaction *act, *
					 * truct sigaction *oact, size_t sigsetsize);                   *
					 * EFAULT:                                                      *
					 * An invalid act or oact value was specified                   *
					 */
ltp_rt_sigaction(signa;
						INVAL_SA_PTR, NULL, SIGSETSIZE));
					if ((TEST_RETURN == -1)
					    && (TEST_ERRNO ==
						test_cases[0].exp_errno)) {
						tst_resm(TINFO,
							 "sa.sa_flags = %s ",
							 test_flags_list[flag]);
						tst_resm(TPASS,
							 "%s failure with sig: %d as expected errno  = %s : %s",
							 TCID, signal,
							 test_cases[0].errdesc,
							 strerror(TEST_ERRNO));
					} else {
						tst_resm(TFAIL,
							 "rt_sigaction call succeeded: result = %ld got error %d:but expected  %d",
							 TEST_RETURN,
							 TEST_ERRNO,
							 test_cases[0].
							 exp_errno);
						tst_resm(TINFO,
							 "sa.sa_flags = %s ",
							 test_flags_list[flag]);
					}
				}
			}
		}
	}
	cleanup();
	tst_exit();
}

