#include "incl.h"
#ifdef HAVE_SYS_XATTR_H
# include <sys/xattr.h>
#endif
char *TCID = "getxattr02";
#ifdef HAVE_SYS_XATTR_H
#define XATTR_TEST_KEY "user.testkey"
#define FIFO "getxattr02fifo"
#define CHR  "getxattr02chr"
#define BLK  "getxattr02blk"
#define SOCK "getxattr02sock"

void setup(void);

void cleanup(void);

char *tc[] = {
};
int TST_TOTAL = sizeof(tc) / sizeof(tc[0]);
int main(int argc, char *argv[])
{
	int lc;
	int i;
	int exp_eno;
	char buf[BUFSIZ];
	tst_parse_opts(argc, argv, NULL, NULL);
	setup();
	for (lc = 0; TEST_LOOPING(lc); lc++) {
		tst_count = 0;
		exp_eno = ENODATA;
		for (i = 0; i < TST_TOTAL; i++) {
getxattr(tc[0], XATTR_TEST_KEY, buf, BUFSIZ);
			if (TEST_RETURN == -1 && TEST_ERRNO == exp_eno)
				tst_resm(TPASS | TTERRNO, "expected behavior");
			else
				tst_resm(TFAIL | TTERRNO, "unexpected behavior"
					 " - expected errno %d - Got", exp_eno);
		}
	}
	cleanup();
	tst_exit();
}

void setup(void)
{
	int fd;
	dev_t dev;
	tst_require_root();
	tst_tmpdir();
	fd = creat(cleanup, "testfile", 0644);
	close(fd);
	if (setxattr("testfile", "user.test", "test", 4, XATTR_CREATE) == -1)
		if (errno == ENOTSUP)
			tst_brkm(TCONF, cleanup, "No xattr support in fs or "
				 "mount without user_xattr option");
	unlink("testfile");
	if (mknod(FIFO, S_IFIFO | 0777, 0) == -1)
		tst_brkm(TBROK | TERRNO, cleanup, "Create FIFO(%s) failed",
			 FIFO);
	dev = makedev(1, 3);
	if (mknod(CHR, S_IFCHR | 0777, dev) == -1)
		tst_brkm(TBROK | TERRNO, cleanup, "Create char special(%s)"
			 " failed", CHR);
	if (mknod(BLK, S_IFBLK | 0777, 0) == -1)
		tst_brkm(TBROK | TERRNO, cleanup, "Create block special(%s)"
			 " failed", BLK);
	if (mknod(SOCK, S_IFSOCK | 0777, 0) == -1)
		tst_brkm(TBROK | TERRNO, cleanup, "Create socket(%s) failed",
			 SOCK);
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

