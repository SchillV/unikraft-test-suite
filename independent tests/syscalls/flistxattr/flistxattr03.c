#include "incl.h"
*  Copyright (c) 2016 RT-RK Institute for Computer Based Systems
*  Author: Dejan Jovicevic <dejan.jovicevic@rt-rk.com>
*/
* Test Name: flistxattr03
*
* Description:
* flistxattr is identical to listxattr. an empty buffer of size zero
* can return the current size of the list of extended attribute names,
* which can be used to estimate a suitable buffer.
*/
#ifdef HAVE_SYS_XATTR_H
# include <sys/xattr.h>
#endif
#ifdef HAVE_SYS_XATTR_H
#define SECURITY_KEY	"security.ltptest"
#define VALUE	"test"
#define VALUE_SIZE	(sizeof(VALUE) - 1)

int fd[] = {0, 0};

int check_suitable_buf(const int file, long size)
{
	int n;
	char buf[size];
	n = flistxattr(file, buf, sizeof(buf));
	return n != -1;
}

int  verify_flistxattr(unsigned int n)
{
flistxattr(fd[n], NULL, 0);
	if (TST_RET == -1) {
		tst_res(TFAIL | TTERRNO, "flistxattr() failed");
		return;
	}
	if (check_suitable_buf(fd[n], TST_RET))
		tst_res(TPASS, "flistxattr() succeed with suitable buffer");
	else
		tst_res(TFAIL, "flistxattr() failed with small buffer");
}

void setup(void)
{
	fd[0] = open("testfile1", O_RDWR | O_CREAT, 0644);
	fd[1] = open("testfile2", O_RDWR | O_CREAT, 0644);
	fsetxattr(fd[1], SECURITY_KEY, VALUE, VALUE_SIZE, XATTR_CREATE);
}

void cleanup(void)
{
	close(fd[1]);
	close(fd[0]);
}

void main(){
	setup();
	cleanup();
}
