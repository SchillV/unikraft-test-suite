#include "incl.h"
#ifdef HAVE_SYS_XATTR_H
# include <sys/xattr.h>
#endif
#ifdef HAVE_SYS_XATTR_H
#define XATTR_TEST_KEY "user.testkey"
#define XATTR_TEST_VALUE "this is a test value"
#define XATTR_TEST_VALUE_SIZE 20
#define FILENAME "fgetxattr03testfile"

int fd = -1;

int  verify_fgetxattr(
{
fgetxattr(fd, XATTR_TEST_KEY, NULL, 0);
	if (TST_RET == XATTR_TEST_VALUE_SIZE) {
		tst_res(TPASS, "fgetxattr(2) returned correct value");
		return;
	}
	tst_res(TFAIL | TTERRNO, "fgetxattr(2) failed with %li", TST_RET);
}

void setup(void)
{
	touch(FILENAME, 0644, NULL);
	fd = open(FILENAME, O_RDONLY);
	fsetxattr(fd, XATTR_TEST_KEY, XATTR_TEST_VALUE,
			XATTR_TEST_VALUE_SIZE, XATTR_CREATE);
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
