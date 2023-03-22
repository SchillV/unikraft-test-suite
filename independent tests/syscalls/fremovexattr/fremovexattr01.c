#include "incl.h"
#ifdef HAVE_SYS_XATTR_H
# include <sys/xattr.h>
#endif
#ifdef HAVE_SYS_XATTR_H
#define ENOATTR ENODATA
#define XATTR_TEST_KEY "user.testkey"
#define XATTR_TEST_VALUE "this is a test value"
#define XATTR_TEST_VALUE_SIZE 20
#define MNTPOINT "mntpoint"
#define FNAME MNTPOINT"/fremovexattr01testfile"

int fd = -1;

char got_value[XATTR_TEST_VALUE_SIZE];

int  verify_fremovexattr(
{
	fsetxattr(fd, XATTR_TEST_KEY, XATTR_TEST_VALUE,
			XATTR_TEST_VALUE_SIZE, XATTR_CREATE);
fremovexattr(fd, XATTR_TEST_KEY);
	if (TST_RET != 0) {
		tst_res(TFAIL | TTERRNO, "fremovexattr(2) failed");
		return;
	}
fgetxattr(fd, XATTR_TEST_KEY, got_value, sizeof(got_value));
	if (TST_RET >= 0) {
		tst_res(TFAIL, "fremovexattr(2) did not remove attribute");
		return;
	}
	if (TST_RET < 0 && TST_ERR != ENOATTR) {
		tst_brk(TBROK | TTERRNO,
int 			"fremovexattr(2) could not verify removal");
		return;
	}
	tst_res(TPASS, "fremovexattr(2) removed attribute as expected");
}

void cleanup(void)
{
	if (fd > 0)
		close(fd);
}

void setup(void)
{
	fd = open(FNAME, O_RDWR | O_CREAT, 0644);
fremovexattr(fd, XATTR_TEST_KEY);
	if (TST_RET == -1 && TST_ERR == EOPNOTSUPP)
		tst_brk(TCONF, "fremovexattr(2) not supported");
}

void main(){
	setup();
	cleanup();
}
