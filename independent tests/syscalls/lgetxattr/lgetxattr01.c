#include "incl.h"
* Copyright (c) 2016 Fujitsu Ltd.
* Author: Jinbao Huang <huangjb.jy@cn.fujitsu.com>
*/
* Test Name: lgetxattr01
*
* Description:
* The testcase checks the basic functionality of the lgetxattr(2).
* In the case of a symbolic link, we only get the value of the
* extended attribute related to the link itself by name.
*
*/
#ifdef HAVE_SYS_XATTR_H
# include <sys/xattr.h>
#endif
#ifdef HAVE_SYS_XATTR_H
#define SECURITY_KEY1   "security.ltptest1"
#define SECURITY_KEY2   "security.ltptest2"
#define VALUE1   "test1"
#define VALUE2   "test2"

void set_xattr(char *path, char *key, void *value, size_t size)
{
	int res;
	res = lsetxattr(path, key, value, size, XATTR_CREATE);
	if (res == -1) {
		if (errno == ENOTSUP) {
			tst_brk(TCONF,
				"no xattr support in fs or mounted "
				"without user_xattr option");
		} else {
			tst_brk(TBROK | TERRNO, "lsetxattr(%s) failed", key);
		}
	}
}

void setup(void)
{
	touch("testfile", 0644, NULL);
	symlink("testfile", "symlink");
	set_xattr("testfile", SECURITY_KEY1, VALUE1, strlen(VALUE1));
	set_xattr("symlink", SECURITY_KEY2, VALUE2, strlen(VALUE2));
}

int  verify_lgetxattr(
{
	int size = 64;
	char buf[size];
lgetxattr("symlink", SECURITY_KEY2, buf, size);
	if (TST_RET == -1) {
		tst_res(TFAIL | TTERRNO, "lgetxattr() failed");
		goto next;
	}
	if (TST_RET != strlen(VALUE2)) {
		tst_res(TFAIL, "lgetxattr() got unexpected value size");
		goto next;
	}
	if (!strncmp(buf, VALUE2, TST_RET))
		tst_res(TPASS, "lgetxattr() got expected value");
	else
		tst_res(TFAIL, "lgetxattr() got unexpected value");
next:
lgetxattr("symlink", SECURITY_KEY1, buf, size);
	if (TST_RET != -1) {
		tst_res(TFAIL, "lgetxattr() succeeded unexpectedly");
		return;
	}
	if (TST_ERR == ENODATA) {
		tst_res(TPASS | TTERRNO, "lgetxattr() failed as expected");
	} else {
		tst_res(TFAIL | TTERRNO, "lgetxattr() failed unexpectedly, expected %s",
			tst_strerrno(ENODATA));
	}
}

void main(){
	setup();
	cleanup();
}
