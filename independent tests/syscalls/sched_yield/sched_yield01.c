#include "incl.h"
char *TCID = "sched_yield01";
int TST_TOTAL = 1;
void setup(void);
void cleanup(void);
int main(int ac, char **av)
{
	int lc;
	tst_parse_opts(ac, av, NULL, NULL);
	setup();
	for (lc = 0; TEST_LOOPING(lc); lc++) {
		tst_count = 0;
sched_yield();
		if (TEST_RETURN != 0) {
			tst_resm(TFAIL, "call failed - errno %d : %s",
				 TEST_ERRNO, strerror(TEST_ERRNO));
		} else {
			tst_resm(TPASS, "sched_yield() call succeeded");
		}
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

