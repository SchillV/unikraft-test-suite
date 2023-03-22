#include "incl.h"
char *TCID = "rt_sigprocmask01";

int testno;
int TST_TOTAL = 8;

volatile sig_atomic_t sig_count;
#define TEST_SIG SIGRTMIN+1

void cleanup(void)
{
	tst_rmdir();
}

void setup(void)
{
	TEST_PAUSE;
	tst_tmpdir();
}
void sig_handler(int sig)
{
	sig_count++;
}
int main(int ac, char **av)
{
	struct sigaction act, oact;
	memset(&act, 0, sizeof(act));
	memset(&oact, 0, sizeof(oact));
	act.sa_handler = sig_handler;
	sigset_t set, oset;
	int lc;
	tst_parse_opts(ac, av, NULL, NULL);
	setup();
	for (lc = 0; TEST_LOOPING(lc); ++lc) {
		tst_count = 0;
		for (testno = 0; testno < TST_TOTAL; ++testno) {
			if (sigemptyset(&set) < 0)
				tst_brkm(TFAIL | TERRNO, cleanup,
					 "sigemptyset call failed");
			if (sigaddset(&set, TEST_SIG) < 0)
				tst_brkm(TFAIL | TERRNO, cleanup,
					 "sigaddset call failed");
ltp_rt_sigaction(TEST_SIG, &act, &oac;
						SIGSETSIZE));
			if (TEST_RETURN < 0)
				tst_brkm(TFAIL | TTERRNO, cleanup,
					 "rt_sigaction call failed");
tst_syscall(__NR_rt_sigprocmask, SIG_BLOCK, &se;
				     &oset, SIGSETSIZE));
			if (TEST_RETURN == -1)
				tst_brkm(TFAIL | TTERRNO, cleanup,
					 "rt_sigprocmask call failed");
			 * masked. */
			if (kill(getpid(), TEST_SIG) < 0)
				tst_brkm(TFAIL | TERRNO, cleanup,
					 "call to kill() failed");
			if (sig_count) {
				tst_brkm(TFAIL | TERRNO, cleanup,
					 "rt_sigprocmask() failed to change "
					 "the process's signal mask");
			} else {
tst_syscall(__NR_rt_sigpending, &ose;
					     SIGSETSIZE));
				if (TEST_RETURN == -1)
					tst_brkm(TFAIL | TTERRNO, cleanup,
						 "rt_sigpending call failed");
sigismember(&oset, TEST_SIG);
				if (TEST_RETURN == 0)
					tst_brkm(TFAIL | TTERRNO,
						 cleanup,
						 "sigismember call failed");
				 * signal#TEST_SIG */
tst_syscall(__NR_rt_sigprocmas;
					     SIG_UNBLOCK, &set, &oset,
					     SIGSETSIZE));
				if (TEST_RETURN == -1)
					tst_brkm(TFAIL | TTERRNO,
						 cleanup,
						 "rt_sigprocmask call failed");
				if (sig_count) {
					tst_resm(TPASS,
						 "rt_sigprocmask "
						 "functionality passed");
					break;
				} else {
					tst_brkm(TFAIL | TERRNO,
						 cleanup,
						 "rt_sigprocmask "
						 "functionality failed");
				}
			}
		}
		tst_count++;
	}
	cleanup();
	tst_exit();
}

