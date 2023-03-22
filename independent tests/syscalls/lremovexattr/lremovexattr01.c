#include "incl.h"
#ifdef HAVE_SYS_XATTR_H
# include <sys/xattr.h>
#endif
#ifdef HAVE_SYS_XATTR_H
#define ENOATTR ENODATA
#define XATTR_KEY		"trusted.key1"
#define XATTR_VALUE		"file and link"
#define XATTR_VALUE_SIZE	13
#define MNTPOINT "mntpoint"
#define FILENAME MNTPOINT"/lremovexattr01testfile"
#define SYMLINK  MNTPOINT"/lremovexattr01symlink"

char got_value[XATTR_VALUE_SIZE];

int  verify_lremovexattr(
{
	setxattr(FILENAME, XATTR_KEY, XATTR_VALUE, XATTR_VALUE_SIZE,
			XATTR_CREATE);
	lsetxattr(SYMLINK, XATTR_KEY, XATTR_VALUE, XATTR_VALUE_SIZE,
			XATTR_CREATE);
lremovexattr(SYMLINK, XATTR_KEY);
	if (TST_RET != 0) {
		tst_res(TFAIL | TTERRNO, "lremovexattr(2) failed");
		return;
	}
	memset(&got_value, 0, XATTR_VALUE_SIZE);
lgetxattr(SYMLINK, XATTR_KEY, &got_value, XATTR_VALUE_SIZE);
	if (TST_RET >= 0) {
		tst_res(TFAIL, "lremovexattr(2) did not work");
		return;
	}
	if (TST_ERR != ENOATTR) {
		tst_brk(TBROK | TTERRNO, "lgetxattr(2) failed unexpectedly");
		return;
	}
	memset(&got_value, 0, XATTR_VALUE_SIZE);
getxattr(FILENAME, XATTR_KEY, &got_value, XATTR_VALUE_SIZE);
	if (TST_RET <= 0) {
		tst_res(TFAIL, "lremovexattr(2) deleted file attribute");
		return;
	}
	if (strncmp(got_value, XATTR_VALUE, XATTR_VALUE_SIZE)) {
		tst_res(TFAIL, "lremovexattr(2) changed file attribute");
		return;
	}
	removexattr(FILENAME, XATTR_KEY);
	tst_res(TPASS, "lremovexattr(2) removed attribute as expected");
}

void setup(void)
{
	touch(FILENAME, 0644, NULL);
	if (symlink(FILENAME, SYMLINK) < 0)
		tst_brk(TCONF, "symlink() not supported");
}

void main(){
	setup();
	cleanup();
}
