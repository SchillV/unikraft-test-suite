#include "incl.h"
#define _GNU_SOURCE
char *TCID = "rt_sigaction01";

int testno;
int TST_TOTAL = 1;

void cleanup(void)
{
	tst_rmdir();
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

void handler(int sig)
{
	tst_resm(TINFO, "Signal Handler Called with signal number %d", sig);
	return;
}

int set_handler(int sig, int sig_to_mask, int mask_flags)
{
	struct sigaction sa, oldaction;
	sa.sa_handler = (void *)handler;
	sa.sa_flags = mask_flags;
	sigemptyset(&sa.sa_mask);
	sigaddset(&sa.sa_mask, sig);
	return ltp_rt_sigaction(sig, &sa, &oldaction, SIGSETSIZE);
}
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
				for (flag = 0;
				     flag <
				      ARRAY_SIZE(test_flags); flag++) {
set_handl;
					     (signal, 0, test_flags[flag]));
					if (TEST_RETURN == 0) {
						tst_resm(TINFO, "signal: %d ",
							 signal);
						tst_resm(TPASS,
							 "rt_sigaction call succeeded: result = %ld ",
							 TEST_RETURN);
						tst_resm(TINFO,
							 "sa.sa_flags = %s ",
							 test_flags_list[flag]);
						kill(getpid(), signal);
					} else {
						tst_resm(TFAIL | TTERRNO,
							 "rt_sigaction call "
							 "failed");
					}
				}
			}
		}
	}
	cleanup();
	tst_exit();
}

