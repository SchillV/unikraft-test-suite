#include "incl.h"
* Copyright (c) 2016 Fujitsu Ltd.
* Author: Jinbao Huang <huangjb.jy@cn.fujitsu.com>
*/
* Test Name: lgetxattr02
*
* Description:
* 1) lgetxattr(2) fails if the named attribute does not exist.
* 2) lgetxattr(2) fails if the size of the value buffer is too small
*    to hold the result.
* 3) lgetxattr(2) fails when attemptes to read from a invalid address.
*
* Expected Result:
* 1) lgetxattr(2) should return -1 and set errno to ENODATA.
* 2) lgetxattr(2) should return -1 and set errno to ERANGE.
* 3) lgetxattr(2) should return -1 and set errno to EFAULT.
*/
#ifdef HAVE_SYS_XATTR_H
# include <sys/xattr.h>
#endif
#ifdef HAVE_SYS_XATTR_H
#define SECURITY_KEY	"security.ltptest"
#define VALUE	"this is a test value"

struct test_case {
	const char *path;
	size_t size;
	int exp_err;
} tcase[] = {
	{"testfile", sizeof(VALUE), ENODATA},
	{"symlink", 1, ERANGE},
	{(char *)-1, sizeof(VALUE), EFAULT}
};

int  verify_lgetxattr(unsigned int n)
{
	struct test_case *tc = tcase + n;
	char buf[tc->size];
lgetxattr(tc->path, SECURITY_KEY, buf, sizeof(buf));
	if (TST_RET != -1) {
		tst_res(TFAIL, "lgetxattr() succeeded unexpectedly");
		return;
	}
	if (TST_ERR != tc->exp_err) {
		tst_res(TFAIL | TTERRNO, "lgetxattr() failed unexpectedlly, "
			"expected %s", tst_strerrno(tc->exp_err));
	} else {
		tst_res(TPASS | TTERRNO, "lgetxattr() failed as expected");
	}
}

void setup(void)
{
	int res;
	touch("testfile", 0644, NULL);
	symlink("testfile", "symlink");
	res = lsetxattr("symlink", SECURITY_KEY, VALUE, strlen(VALUE), XATTR_CREATE);
	if (res == -1) {
		if (errno == ENOTSUP) {
			tst_brk(TCONF, "no xattr support in fs or "
				"mounted without user_xattr option");
		} else {
			tst_brk(TBROK | TERRNO, "lsetxattr(%s) failed",
				SECURITY_KEY);
		}
	}
}

void main(){
	setup();
	cleanup();
}
