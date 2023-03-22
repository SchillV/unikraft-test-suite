#include "incl.h"
*  Copyright (c) 2016 RT-RK Institute for Computer Based Systems
*  Author: Dejan Jovicevic <dejan.jovicevic@rt-rk.com>
*/
* Test Name: listxattr02
*
* Description:
* 1) listxattr(2) fails if the size of the list buffer is too small
* to hold the result.
* 2) listxattr(2) fails if path is an empty string.
* 3) listxattr(2) fails when attempted to read from a invalid address.
* 4) listxattr(2) fails if path is longer than allowed.
*
* Expected Result:
* 1) listxattr(2) should return -1 and set errno to ERANGE.
* 2) listxattr(2) should return -1 and set errno to ENOENT.
* 3) listxattr(2) should return -1 and set errno to EFAULT.
* 4) listxattr(2) should return -1 and set errno to ENAMETOOLONG.
*/
#ifdef HAVE_SYS_XATTR_H
# include <sys/xattr.h>
#endif
#ifdef HAVE_SYS_XATTR_H
#define SECURITY_KEY	"security.ltptest"
#define VALUE	"test"
#define VALUE_SIZE	(sizeof(VALUE) - 1)
#define TESTFILE    "testfile"
char longpathname[PATH_MAX + 2];

struct test_case {
	const char *path;
	size_t size;
	int exp_err;
} tc[] = {
	{TESTFILE, 1, ERANGE},
	{"", 20, ENOENT},
	{(char *)-1, 20, EFAULT},
	{longpathname, 20, ENAMETOOLONG}
};

int  verify_listxattr(unsigned int n)
{
	struct test_case *t = tc + n;
	char buf[t->size];
listxattr(t->path, buf, sizeof(buf));
	if (TST_RET != -1) {
		tst_res(TFAIL,
			"listxattr() succeeded unexpectedly (returned %ld)",
			TST_RET);
		return;
	}
	if (t->exp_err != TST_ERR) {
		tst_res(TFAIL | TTERRNO, "listxattr() failed "
			 "unexpectedlly, expected %s",
			 tst_strerrno(t->exp_err));
	} else {
		tst_res(TPASS | TTERRNO,
			 "listxattr() failed as expected");
	}
}

void setup(void)
{
	touch(TESTFILE, 0644, NULL);
	setxattr(TESTFILE, SECURITY_KEY, VALUE, VALUE_SIZE, XATTR_CREATE);
	memset(&longpathname, 'a', sizeof(longpathname) - 1);
}

void main(){
	setup();
	cleanup();
}
