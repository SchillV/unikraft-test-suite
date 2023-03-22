#include "incl.h"
char *TCID = "personality02";
int TST_TOTAL = 1;
#define USEC 10

int  verify_personality(
{
	struct timeval tv = {.tv_sec = 0, .tv_usec = USEC};
	int ret;
	fd_set rfds;
	FD_ZERO(&rfds);
	FD_SET(1, &rfds);
	personality(PER_LINUX | STICKY_TIMEOUTS);
	ret = select(2, &rfds, NULL, NULL, &tv);
	personality(PER_LINUX);
	if (ret < 0)
		tst_resm(TBROK | TERRNO, "select()");
	if (tv.tv_usec != USEC)
		tst_resm(TFAIL, "Timeout was modified");
	else
		tst_resm(TPASS, "Timeout wasn't modified");
}
int main(int ac, char **av)
{
	int lc;
	tst_parse_opts(ac, av, NULL, NULL);
	for (lc = 0; TEST_LOOPING(lc); lc++)
int 		verify_personality();
	tst_exit();
}

