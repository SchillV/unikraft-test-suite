#include "incl.h"
#ifdef HAVE_SYS_XATTR_H
# include <sys/xattr.h>
#endif
#ifdef HAVE_SYS_XATTR_H
#define XATTR_TEST_KEY "user.testkey"
#define MNTPOINT "mntpoint"
#define FNAME MNTPOINT"/fremovexattr02testfile"

int fd = -1;
struct test_case {
	int fd;
	char *key;
	int exp_err;
};
struct test_case tc[] = {
	 .key = XATTR_TEST_KEY,
	 .exp_err = ENODATA,
	 },
	 .fd = -1,
	 .key = XATTR_TEST_KEY,
	 .exp_err = EBADF,
	 },
	 .exp_err = EFAULT,
	},
};

int  verify_fremovexattr(unsigned int i)
{
fremovexattr(tc[i].fd, tc[i].key);
	if (TST_RET == -1 && TST_ERR == EOPNOTSUPP)
		tst_brk(TCONF, "fremovexattr(2) not supported");
	if (TST_RET == -1) {
		if (tc[i].exp_err == TST_ERR) {
			tst_res(TPASS | TTERRNO,
				"fremovexattr(2) failed expectedly");
		} else {
			tst_res(TFAIL | TTERRNO,
				"fremovexattr(2) should fail with %s",
				tst_strerrno(tc[i].exp_err));
		}
		return;
	}
	tst_res(TFAIL, "fremovexattr(2) returned %li", TST_RET);
}

void cleanup(void)
{
	if (fd > 0)
		close(fd);
}

void setup(void)
{
	size_t i = 0;
	fd = open(FNAME, O_RDWR | O_CREAT, 0644);
	for (i = 0; i < ARRAY_SIZE(tc); i++) {
		if (tc[i].fd != -1)
			tc[i].fd = fd;
		if (!tc[i].key && tc[i].exp_err == EFAULT)
			tc[i].key = tst_get_bad_addr(cleanup);
	}
}

void main(){
	setup();
	cleanup();
}
