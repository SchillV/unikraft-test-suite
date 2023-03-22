#include "incl.h"
* Copyright (c) 2016 Fujitsu Ltd.
* Author: Xiao Yang <yangx.jy@cn.fujitsu.com>
*/
* Test Name: llistxattr02
*
* Description:
* 1) llistxattr(2) fails if the size of the list buffer is too small
* to hold the result.
* 2) llistxattr(2) fails if path is an empty string.
* 3) llistxattr(2) fails when attempted to read from a invalid address.
* 4) llistxattr(2) fails if path is longer than allowed.
*
* Expected Result:
* 1) llistxattr(2) should return -1 and set errno to ERANGE.
* 2) llistxattr(2) should return -1 and set errno to ENOENT.
* 3) llistxattr(2) should return -1 and set errno to EFAULT.
* 4) llistxattr(2) should return -1 and set errno to ENAMETOOLONG.
*/
#ifdef HAVE_SYS_XATTR_H
# include <sys/xattr.h>
#endif
#ifdef HAVE_SYS_XATTR_H
#define SECURITY_KEY    "security.ltptest"
#define VALUE           "test"
#define VALUE_SIZE      (sizeof(VALUE) - 1)
#define TESTFILE        "testfile"
#define SYMLINK         "symlink"

char longpathname[PATH_MAX + 2];

struct test_case {
	const char *path;
	size_t size;
	int exp_err;
} tc[] = {
	{SYMLINK, 1, ERANGE},
	{"", 20, ENOENT},
	{(char *)-1, 20, EFAULT},
	{longpathname, 20, ENAMETOOLONG}
};

int  verify_llistxattr(unsigned int n)
{
	struct test_case *t = tc + n;
	char buf[t->size];
llistxattr(t->path, buf, sizeof(buf));
	if (TST_RET != -1) {
		tst_res(TFAIL,
			"llistxattr() succeeded unexpectedly (returned %ld)",
			TST_RET);
		return;
	}
	if (TST_ERR != t->exp_err) {
		tst_res(TFAIL | TTERRNO, "llistxattr() failed "
			 "unexpectedlly, expected %s",
			 tst_strerrno(t->exp_err));
	} else {
		tst_res(TPASS | TTERRNO,
			 "llistxattr() failed as expected");
	}
}

void setup(void)
{
	touch(TESTFILE, 0644, NULL);
	symlink(TESTFILE, SYMLINK);
	lsetxattr(SYMLINK, SECURITY_KEY, VALUE, VALUE_SIZE, XATTR_CREATE);
	memset(longpathname, 'a', sizeof(longpathname) - 1);
}

void main(){
	setup();
	cleanup();
}
