#include "incl.h"
* Copyright (c) 2016 Fujitsu Ltd.
* Author: Xiao Yang <yangx.jy@cn.fujitsu.com>
*/
* Test Name: llistxattr03
*
* Description:
* llistxattr is identical to listxattr. an empty buffer of size zero
* can return the current size of the list of extended attribute names,
* which can be used to estimate a suitable buffer.
*/
#ifdef HAVE_SYS_XATTR_H
#endif
#ifdef HAVE_SYS_XATTR_H
#define SECURITY_KEY	"security.ltptest"
#define VALUE           "test"
#define VALUE_SIZE      (sizeof(VALUE) - 1)

const char *filename[] = {"testfile1", "testfile2"};

int check_suitable_buf(const char *name, long size)
{
	int n;
	char buf[size];
	n = llistxattr(name, buf, sizeof(buf));
	return n != -1;
}

int  verify_llistxattr(unsigned int n)
{
	const char *name = filename[n];
llistxattr(name, NULL, 0);
	if (TST_RET == -1) {
		tst_res(TFAIL | TTERRNO, "llistxattr() failed");
		return;
	}
	if (check_suitable_buf(name, TST_RET))
		tst_res(TPASS, "llistxattr() succeed with suitable buffer");
	else
		tst_res(TFAIL, "llistxattr() failed with small buffer");
}

void setup(void)
{
	touch(filename[0], 0644, NULL);
	touch(filename[1], 0644, NULL);
	lsetxattr(filename[1], SECURITY_KEY, VALUE, VALUE_SIZE, XATTR_CREATE);
}

void main(){
	setup();
	cleanup();
}
