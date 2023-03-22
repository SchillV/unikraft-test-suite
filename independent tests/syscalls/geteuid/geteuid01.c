#include "incl.h"

void setup(void);

void cleanup(void);
TCID_DEFINE(geteuid01);
int TST_TOTAL = 1;
int main(int ac, char **av)
{
	int lc;
	tst_parse_opts(ac, av, NULL, NULL);
	setup();
	for (lc = 0; TEST_LOOPING(lc); lc++) {
		tst_count = 0;
GETEUID(cleanup);
		if (TEST_RETURN < 0) {
			tst_resm(TFAIL | TTERRNO, "geteuid failed");
		}
		tst_resm(TPASS, "geteuid returned %ld", TEST_RETURN);
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

