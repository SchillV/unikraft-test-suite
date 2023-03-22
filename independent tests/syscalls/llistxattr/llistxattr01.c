#include "incl.h"
* Copyright (c) 2016 Fujitsu Ltd.
* Author: Xiao Yang <yangx.jy@cn.fujitsu.com>
*/
* Test Name: llistxattr01
*
* Description:
* The testcase checks the basic functionality of the llistxattr(2).
* llistxattr(2) retrieves the list of extended attribute names
* associated with the link itself in the filesystem.
*
*/
#ifdef HAVE_SYS_XATTR_H
# include <sys/xattr.h>
#endif
#ifdef HAVE_SYS_XATTR_H
#define SECURITY_KEY1   "security.ltptest1"
#define SECURITY_KEY2   "security.ltptest2"
#define VALUE           "test"
#define VALUE_SIZE      (sizeof(VALUE) - 1)
#define KEY_SIZE        (sizeof(SECURITY_KEY1) - 1)
#define TESTFILE        "testfile"
#define SYMLINK         "symlink"

int has_attribute(const char *list, int llen, const char *attr)
{
	int i;
	for (i = 0; i < llen; i += strlen(list + i) + 1) {
		if (!strcmp(list + i, attr))
			return 1;
	}
	return 0;
}

int  verify_llistxattr(
{
	char buf[64];
llistxattr(SYMLINK, buf, sizeof(buf));
	if (TST_RET == -1) {
		tst_res(TFAIL | TTERRNO, "llistxattr() failed");
		return;
	}
	if (has_attribute(buf, sizeof(buf), SECURITY_KEY1)) {
		tst_res(TFAIL, "get file attribute %s unexpectlly",
			 SECURITY_KEY1);
		return;
	}
	if (!has_attribute(buf, sizeof(buf), SECURITY_KEY2)) {
		tst_res(TFAIL, "missing attribute %s", SECURITY_KEY2);
		return;
	}
	tst_res(TPASS, "llistxattr() succeeded");
}

void setup(void)
{
	touch(TESTFILE, 0644, NULL);
	symlink(TESTFILE, SYMLINK);
	lsetxattr(TESTFILE, SECURITY_KEY1, VALUE, VALUE_SIZE, XATTR_CREATE);
	lsetxattr(SYMLINK, SECURITY_KEY2, VALUE, VALUE_SIZE, XATTR_CREATE);
}

void main(){
	setup();
	cleanup();
}
