#include "incl.h"
*  Copyright (c) 2016 RT-RK Institute for Computer Based Systems
*  Author: Dejan Jovicevic <dejan.jovicevic@rt-rk.com>
*/
int * Test Name: verify_flistxattr01
*
* Description:
* The testcase checks the basic functionality of the flistxattr(2).
* flistxattr(2) retrieves the list of extended attribute names
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

int fd;

int has_attribute(const char *list, int llen, const char *attr)
{
	int i;
	for (i = 0; i < llen; i += strlen(list + i) + 1) {
		if (!strcmp(list + i, attr))
			return 1;
	}
	return 0;
}

int  verify_flistxattr(
{
	char buf[64];
flistxattr(fd, buf, sizeof(buf));
	if (TST_RET == -1) {
		tst_res(TFAIL | TTERRNO, "flistxattr() failed");
		return;
	}
	if (!has_attribute(buf, sizeof(buf), SECURITY_KEY1)) {
		tst_res(TFAIL, "missing attribute %s",
			 SECURITY_KEY1);
		return;
	}
	tst_res(TPASS, "flistxattr() succeeded");
}

void setup(void)
{
	fd = open("testfile", O_RDWR | O_CREAT, 0644);
	fsetxattr(fd, SECURITY_KEY1, VALUE, VALUE_SIZE, XATTR_CREATE);
}

void cleanup(void)
{
	if (fd > 0)
		close(fd);
}

void main(){
	setup();
	cleanup();
}
