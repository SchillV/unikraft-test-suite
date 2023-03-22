#include "incl.h"
* Copyright (c) 2016 RT-RK Institute for Computer Based Systems
* Author: Dejan Jovicevic <dejan.jovicevic@rt-rk.com>
*/
* Test Name: flistxattr02
*
* Description:
* 1) flistxattr(2) fails if the size of the list buffer is too small
* to hold the result.
* 2) flistxattr(2) fails if fd is an invalid file descriptor.
*
* Expected Result:
* 1) flistxattr(2) should return -1 and set errno to ERANGE.
* 2) flistxattr(2) should return -1 and set errno to EBADF.
*/
#ifdef HAVE_SYS_XATTR_H
# include <sys/xattr.h>
#endif
#ifdef HAVE_SYS_XATTR_H
#define SECURITY_KEY	"security.ltptest"
#define VALUE	"test"
#define VALUE_SIZE	(sizeof(VALUE) - 1)

int fd1;

int fd2 = -1;

struct test_case {
	int *fd;
	size_t size;
	int exp_err;
} tc[] = {
	{&fd1, 1, ERANGE},
	{&fd2, 20, EBADF}
};

int  verify_flistxattr(unsigned int n)
{
	struct test_case *t = tc + n;
	char buf[t->size];
flistxattr(*t->fd, buf, t->size);
	if (TST_RET != -1) {
		tst_res(TFAIL,
			"flistxattr() succeeded unexpectedly (returned %ld)",
			TST_RET);
		return;
	}
	if (t->exp_err != TST_ERR) {
		tst_res(TFAIL | TTERRNO, "flistxattr() failed "
			 "unexpectedlly, expected %s",
			 tst_strerrno(t->exp_err));
	} else {
		tst_res(TPASS | TTERRNO,
			 "flistxattr() failed as expected");
	}
}

void setup(void)
{
	fd1 = open("testfile", O_RDWR | O_CREAT, 0644);
	fsetxattr(fd1, SECURITY_KEY, VALUE, VALUE_SIZE, XATTR_CREATE);
}

void cleanup(void)
{
	if (fd1 > 0)
		close(fd1);
}

void main(){
	setup();
	cleanup();
}
