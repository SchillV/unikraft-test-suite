#include "incl.h"

void cleanup(void);

void setup(void);
TCID_DEFINE(getegid02);
int TST_TOTAL = 1;
int main(int ac, char **av)
{
	int lc;
	uid_t euid;
	struct passwd *pwent;
	tst_parse_opts(ac, av, NULL, NULL);
	setup();
	for (lc = 0; TEST_LOOPING(lc); lc++) {
		tst_count = 0;
GETEGID(cleanup);
		if (TEST_RETURN < 0) {
			tst_brkm(TBROK, cleanup, "This should never happen");
		}
		euid = geteuid();
		pwent = getpwuid(euid);
		if (pwent == NULL)
			tst_brkm(TBROK, cleanup, "geteuid() returned "
				 "unexpected value %d", euid);
		GID16_CHECK(pwent->pw_gid, getegid, cleanup);
		if (pwent->pw_gid != TEST_RETURN) {
			tst_resm(TFAIL, "getegid() return value"
				 " %ld unexpected - expected %d",
				 TEST_RETURN, pwent->pw_gid);
		} else {
			tst_resm(TPASS,
				 "effective group id %ld "
				 "is correct", TEST_RETURN);
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

