#include "incl.h"
*  Copyright (c) 2016 RT-RK Institute for Computer Based Systems
*  Author: Dejan Jovicevic <dejan.jovicevic@rt-rk.com>
*/
* Test Name: listxattr01
*
* Description:
* The testcase checks the basic functionality of the listxattr(2).
* listxattr(2) retrieves the list of extended attribute names
* associated with the file itself in the filesystem.
*
*/
#ifdef HAVE_SYS_XATTR_H
# include <sys/xattr.h>
#endif
#ifdef HAVE_SYS_XATTR_H
#define SECURITY_KEY1	"security.ltptest1"
#define VALUE	"test"
#define VALUE_SIZE	(sizeof(VALUE) - 1)
#define KEY_SIZE    (sizeof(SECURITY_KEY1) - 1)
#define TESTFILE    "testfile"

int has_attribute(const char *list, int llen, const char *attr)
{
	int i;
	for (i = 0; i < llen; i += strlen(list + i) + 1) {
		if (!strcmp(list + i, attr))
			return 1;
	}
	return 0;
}

int  verify_listxattr(
{
	char buf[64];
listxattr(TESTFILE, buf, sizeof(buf));
	if (TST_RET == -1) {
		tst_res(TFAIL | TTERRNO, "listxattr() failed");
		return;
	}
	if (!has_attribute(buf, sizeof(buf), SECURITY_KEY1)) {
		tst_res(TFAIL, "missing attribute %s",
			 SECURITY_KEY1);
		return;
	}
	tst_res(TPASS, "listxattr() succeeded");
}

void setup(void)
{
	touch(TESTFILE, 0644, NULL);
	setxattr(TESTFILE, SECURITY_KEY1, VALUE, VALUE_SIZE, XATTR_CREATE);
}

void main(){
	setup();
	cleanup();
}
