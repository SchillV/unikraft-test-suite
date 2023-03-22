#include "incl.h"
TCID_DEFINE(geteuid02);
int TST_TOTAL = 1;

void setup(void);

void cleanup(void);
int main(int ac, char **av)
{
	struct passwd *pwent;
	int lc;
	uid_t uid;
	tst_parse_opts(ac, av, NULL, NULL);
	setup();
	for (lc = 0; TEST_LOOPING(lc); lc++) {
		tst_count = 0;
GETEUID(cleanup);
		if (TEST_RETURN == -1)
			tst_brkm(TBROK | TTERRNO, cleanup, "geteuid* failed");
		uid = geteuid();
		pwent = getpwuid(uid);
		if (pwent == NULL)
			tst_resm(TFAIL | TERRNO, "getpwuid failed");
		UID16_CHECK(pwent->pw_uid, geteuid, cleanup);
		if (pwent->pw_uid != TEST_RETURN)
			tst_resm(TFAIL, "getpwuid value, %d, "
				 "does not match geteuid "
				 "value, %ld", pwent->pw_uid,
				 TEST_RETURN);
		else
			tst_resm(TPASS, "values from geteuid "
				 "and getpwuid match");
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

