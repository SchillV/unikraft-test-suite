#include "incl.h"
#ifdef HAVE_SYS_XATTR_H
# include <sys/xattr.h>
#endif
char *TCID = "getxattr03";
#ifdef HAVE_SYS_XATTR_H
#define XATTR_TEST_KEY "user.testkey"
#define XATTR_TEST_VALUE "test value"
#define XATTR_TEST_VALUE_SIZE (sizeof(XATTR_TEST_VALUE) - 1)
#define TESTFILE "getxattr03testfile"

void setup(void);

void cleanup(void);
int TST_TOTAL = 1;
int main(int argc, char *argv[])
{
	int lc;
	tst_parse_opts(argc, argv, NULL, NULL);
	setup();
	for (lc = 0; TEST_LOOPING(lc); lc++) {
		tst_count = 0;
getxattr(TESTFILE, XATTR_TEST_KEY, NULL, 0);
		if (TEST_RETURN == XATTR_TEST_VALUE_SIZE)
			tst_resm(TPASS, "getxattr(2) returned correct value");
		else
			tst_resm(TFAIL | TTERRNO, "getxattr(2) failed");
	}
	cleanup();
	tst_exit();
}

void setup(void)
{
	int fd;
	tst_require_root();
	tst_tmpdir();
	fd = creat(cleanup, TESTFILE, 0644);
	close(fd);
	if (setxattr(TESTFILE, XATTR_TEST_KEY, XATTR_TEST_VALUE,
		     XATTR_TEST_VALUE_SIZE, XATTR_CREATE) == -1) {
		if (errno == ENOTSUP)
			tst_brkm(TCONF, cleanup, "No xattr support in fs or "
				 "fs mounted without user_xattr option");
		else
			tst_brkm(TBROK | TERRNO, cleanup, "setxattr %s failed",
				 TESTFILE);
	}
	TEST_PAUSE;
}

void cleanup(void)
{
	tst_rmdir();
}
int main(int argc, char *argv[])
{
	tst_brkm(TCONF, NULL, "<sys/xattr.h> does not exist.");
}
#endif

