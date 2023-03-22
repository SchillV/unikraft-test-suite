#include "incl.h"
#define _GNU_SOURCE 1
void setup(void);
void cleanup(void);
char *TCID = "getpgid01";
int TST_TOTAL = 1;
int main(int ac, char **av)
{
	int lc;
	register int pgid_0, pgid_1;
	register int my_pid, my_ppid;
	int ex_stat;
	tst_parse_opts(ac, av, NULL, NULL);
	setup();
	for (lc = 0; TEST_LOOPING(lc); lc++) {
		tst_count = 0;
		if ((pgid_0 = FORK_OR_VFORK()) == -1)
			tst_brkm(TBROK, cleanup, "fork failed");
		if (pgid_0 > 0) {
			while ((pgid_0 = wait(&ex_stat)) != -1) ;
			if (WEXITSTATUS(ex_stat) == 0)
				tst_resm(TPASS, "%s PASSED", TCID);
			else
				tst_resm(TFAIL, "%s FAILED", TCID);
			exit(0);
		}
		if ((pgid_0 = getpgid(0)) == -1)
			tst_resm(TFAIL | TERRNO, "getpgid(0) failed");
		else
			tst_resm(TPASS, "getpgid(0) PASSED");
		my_pid = getpid();
		if ((pgid_1 = getpgid(my_pid)) == -1)
			tst_resm(TFAIL | TERRNO, "getpgid(%d) failed", my_pid);
		if (pgid_0 != pgid_1) {
			tst_resm(TFAIL, "getpgid(my_pid=%d) != getpgid(0) "
				 "[%d != %d]", my_pid, pgid_1, pgid_0);
		} else
			tst_resm(TPASS, "getpgid(getpid()) PASSED");
		my_ppid = getppid();
		if ((pgid_1 = getpgid(my_ppid)) == -1)
			tst_resm(TFAIL | TERRNO, "getpgid(%d) failed", my_ppid);
		if (pgid_0 != pgid_1) {
			tst_resm(TFAIL, "getpgid(%d) != getpgid(0) [%d != %d]",
				 my_ppid, pgid_1, pgid_0);
		} else
			tst_resm(TPASS, "getpgid(getppid()) PASSED");
		if ((pgid_1 = getpgid(pgid_0)) < 0)
			tst_resm(TFAIL | TERRNO, "getpgid(%d) failed", pgid_0);
		if (pgid_0 != pgid_1) {
			tst_resm(TFAIL, "getpgid(%d) != getpgid(0) [%d != %d]",
				 pgid_0, pgid_1, pgid_0);
		} else
			tst_resm(TPASS, "getpgid(%d) PASSED", pgid_0);
		if (getpgid(1) < 0)
			tst_resm(TFAIL | TERRNO, "getpgid(1) failed");
		else
			tst_resm(TPASS, "getpgid(1) PASSED");
	}
	cleanup();
	tst_exit();
}
void setup(void)
{
	tst_sig(FORK, DEF_HANDLER, cleanup);
	TEST_PAUSE;
}
void cleanup(void)
{
}

