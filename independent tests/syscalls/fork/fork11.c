#include "incl.h"
char *TCID = "fork11";
int TST_TOTAL = 1;

void setup(void);

void cleanup(void);
#define NUMFORKS 100
int main(int ac, char **av)
{
	int i, pid, cpid, status;
	int fail = 0;
	int lc;
	tst_parse_opts(ac, av, NULL, NULL);
	setup();
	for (lc = 0; TEST_LOOPING(lc); lc++) {
		tst_count = 0;
		for (i = 0; i < NUMFORKS; i++) {
			pid = fork();
			if (pid == 0)
				exit(0);
				cpid = wait(&status);
				if (cpid != pid)
					fail++;
			} else {
				fail++;
				break;
			}
		}
		if (fail)
			tst_resm(TFAIL, "fork failed %d times", fail);
		else
			tst_resm(TPASS, "fork test passed, %d processes", i);
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

