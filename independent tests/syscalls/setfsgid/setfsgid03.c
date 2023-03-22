#include "incl.h"
TCID_DEFINE(setfsgid03);
int TST_TOTAL = 1;

char nobody_uid[] = "nobody";

struct passwd *ltpuser;

void setup(void);

void cleanup(void);
int main(int ac, char **av)
{
	int lc;
	gid_t gid;
	tst_parse_opts(ac, av, NULL, NULL);
	setup();
	for (lc = 0; TEST_LOOPING(lc); lc++) {
		tst_count = 0;
		gid = 1;
		while (!getgrgid(gid))
			gid++;
		GID16_CHECK(gid, setfsgid, cleanup);
SETFSGID(cleanup, gid);
		if (TEST_RETURN == -1) {
			tst_resm(TFAIL | TTERRNO,
				"setfsgid() failed unexpectedly");
			continue;
		}
		if (TEST_RETURN == gid) {
			tst_resm(TFAIL,
				 "setfsgid() returned %ld, expected anything but %d",
				 TEST_RETURN, gid);
		} else {
			tst_resm(TPASS, "setfsgid() returned expected value : "
				 "%ld", TEST_RETURN);
		}
	}
	cleanup();
	tst_exit();
}

void setup(void)
{
	tst_require_root();
	ltpuser = getpwnam(nobody_uid);
	if (ltpuser == NULL)
		tst_brkm(TBROK, cleanup, "getpwnam failed for user id %s",
			nobody_uid);
	if (setuid(ltpuser->pw_uid) == -1)
		tst_resm(TINFO | TERRNO,
			"setuid failed to set the effective uid to %d",
			ltpuser->pw_uid);
	tst_sig(NOFORK, DEF_HANDLER, cleanup);
	TEST_PAUSE;
}

void cleanup(void)
{
}

