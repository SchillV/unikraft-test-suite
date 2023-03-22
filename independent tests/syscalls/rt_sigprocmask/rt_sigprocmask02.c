#include "incl.h"
char *TCID = "rt_sigprocmask02";
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

sigset_t set;

struct test_case_t {
	sigset_t *ss;
	int sssize;
	int exp_errno;
} test_cases[] = {
	{
	&set, 1, EINVAL}, {
	(sigset_t *) - 1, SIGSETSIZE, EFAULT}
};
int test_count = sizeof(test_cases) / sizeof(struct test_case_t);
int main(int ac, char **av)
{
	int i;
	sigset_t s;
	tst_parse_opts(ac, av, NULL, NULL);
	setup();
	tst_count = 0;
sigfillset(&s);
	if (TEST_RETURN == -1)
		tst_brkm(TFAIL | TTERRNO, cleanup,
			"Call to sigfillset() failed.");
	for (i = 0; i < test_count; i++) {
tst_syscall(__NR_rt_sigprocmask, SIG_BLOC;
			     &s, test_cases[i].ss, test_cases[i].sssize));
		if (TEST_RETURN == 0) {
			tst_resm(TFAIL | TTERRNO,
				 "Call to rt_sigprocmask() succeeded, "
				 "but should failed");
		} else if (TEST_ERRNO == test_cases[i].exp_errno) {
			tst_resm(TPASS | TTERRNO, "Got expected errno");
		} else {
			tst_resm(TFAIL | TTERRNO, "Got unexpected errno");
		}
	}
	cleanup();
	tst_exit();
}

