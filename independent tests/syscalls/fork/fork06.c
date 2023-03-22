#include "incl.h"
char *TCID = "fork06";
int TST_TOTAL = 1;

void setup(void);

void cleanup(void);
#define NUMFORKS 1000
int main(int ac, char **av)
{
	int i, pid, status, childpid, succeed, fail;
	int lc;
	tst_parse_opts(ac, av, NULL, NULL);
	setup();
	for (lc = 0; TEST_LOOPING(lc); lc++) {
		tst_count = 0;
		succeed = 0;
		fail = 0;
		for (i = 0; i < NUMFORKS; i++) {
			pid = fork();
			if (pid == -1) {
				fail++;
				continue;
			}
			if (pid == 0)
				_exit(0);
			childpid = wait(&status);
			if (pid != childpid)
				tst_resm(TFAIL, "pid from wait %d", childpid);
			succeed++;
		}
		tst_resm(TINFO, "tries %d", i);
		tst_resm(TINFO, "successes %d", succeed);
		tst_resm(TINFO, "failures %d", fail);
		if ((wait(&status)) == -1)
			tst_resm(TINFO, "There were no children to wait for");
		else
			tst_resm(TINFO, "There were children left");
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

