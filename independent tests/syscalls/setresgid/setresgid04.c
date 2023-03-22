#include "incl.h"
#define _GNU_SOURCE
TCID_DEFINE(setresgid04);
int TST_TOTAL = 1;

struct passwd *ltpuser;

void setup(void);

int  setresgid_verify(

void cleanup(void);
int main(int argc, char **argv)
{
	int i, lc;
	tst_parse_opts(argc, argv, NULL, NULL);
	setup();
	for (lc = 0; TEST_LOOPING(lc); lc++) {
		tst_count = 0;
		for (i = 0; i < TST_TOTAL; i++)
int 			setresgid_verify();
	}
	cleanup();
	tst_exit();
}

void setup(void)
{
	tst_require_root();
	tst_sig(NOFORK, DEF_HANDLER, cleanup);
	TEST_PAUSE;
	tst_tmpdir();
	ltpuser = getpwnam(cleanup, "nobody");
	GID16_CHECK(ltpuser->pw_gid, "setresgid", cleanup)
}

int  setresgid_verify(
{
	struct stat buf;
SETRESGID(cleanup, -1, ltpuser->pw_gid, -1);
	if (TEST_RETURN != 0) {
		tst_resm(TFAIL | TTERRNO, "setresgid failed unexpectedly");
		return;
	}
	touch(cleanup, "test_file", 0644, NULL);
	stat(cleanup, "test_file", &buf);
	if (ltpuser->pw_gid == buf.st_gid) {
		tst_resm(TPASS, "setresgid succeeded as expected");
	} else {
		tst_resm(TFAIL,
			 "setresgid failed unexpectedly; egid(%d) - st_gid(%d)",
			 ltpuser->pw_gid, buf.st_gid);
	}
}

void cleanup(void)
{
	tst_rmdir();
}

