#include "incl.h"
#ifdef HAVE_SYS_XATTR_H
# include <sys/xattr.h>
#endif
#ifdef HAVE_SYS_XATTR_H
#define XATTR_TEST_KEY "user.testkey"
#define XATTR_TEST_VALUE "this is a test value"
#define XATTR_TEST_VALUE_SIZE 20
#define OFFSET    10
#define FILENAME "setxattr02testfile"
#define DIRNAME  "setxattr02testdir"
#define SYMLINK  "setxattr02symlink"
#define FIFO     "setxattr02fifo"
#define CHR      "setxattr02chr"
#define BLK      "setxattr02blk"
#define SOCK     "setxattr02sock"
struct test_case {
	char *fname;
	char *key;
	char *value;
	size_t size;
	int flags;
	int exp_err;
	int needskeyset;
};

struct test_case tc[] = {
	 .fname = FILENAME,
	 .key = XATTR_TEST_KEY,
	 .value = XATTR_TEST_VALUE,
	 .size = XATTR_TEST_VALUE_SIZE,
	 .flags = XATTR_CREATE,
	 .exp_err = 0,
	 },
	 .fname = DIRNAME,
	 .key = XATTR_TEST_KEY,
	 .value = XATTR_TEST_VALUE,
	 .size = XATTR_TEST_VALUE_SIZE,
	 .flags = XATTR_CREATE,
	 .exp_err = 0,
	 },
	 .fname = SYMLINK,
	 .key = XATTR_TEST_KEY,
	 .value = XATTR_TEST_VALUE,
	 .size = XATTR_TEST_VALUE_SIZE,
	 .flags = XATTR_CREATE,
	 .exp_err = EEXIST,
	 .needskeyset = 1,
	 },
	 .fname = FIFO,
	 .key = XATTR_TEST_KEY,
	 .value = XATTR_TEST_VALUE,
	 .size = XATTR_TEST_VALUE_SIZE,
	 .flags = XATTR_CREATE,
	 .exp_err = EPERM,
	 },
	 .fname = CHR,
	 .key = XATTR_TEST_KEY,
	 .value = XATTR_TEST_VALUE,
	 .size = XATTR_TEST_VALUE_SIZE,
	 .flags = XATTR_CREATE,
	 .exp_err = EPERM,
	 },
	 .fname = BLK,
	 .key = XATTR_TEST_KEY,
	 .value = XATTR_TEST_VALUE,
	 .size = XATTR_TEST_VALUE_SIZE,
	 .flags = XATTR_CREATE,
	 .exp_err = EPERM,
	 },
	 .fname = SOCK,
	 .key = XATTR_TEST_KEY,
	 .value = XATTR_TEST_VALUE,
	 .size = XATTR_TEST_VALUE_SIZE,
	 .flags = XATTR_CREATE,
	 .exp_err = EPERM,
	 },
};

int  verify_setxattr(unsigned int i)
{
	if (tc[i].needskeyset) {
		setxattr(tc[i].fname, tc[i].key, tc[i].value, tc[i].size,
				XATTR_CREATE);
	}
setxattr(tc[i].fname, tc[i].key, tc[i].value, tc[i].siz;
			tc[i].flags));
	if (TST_RET == -1 && TST_ERR == EOPNOTSUPP)
		tst_brk(TCONF, "setxattr(2) not supported");
	if (!tc[i].exp_err) {
		if (TST_RET) {
			tst_res(TFAIL | TTERRNO,
				"setxattr(2) on %s failed with %li",
				tc[i].fname + OFFSET, TST_RET);
			return;
		}
		removexattr(tc[i].fname, tc[i].key);
		tst_res(TPASS, "setxattr(2) on %s passed",
				tc[i].fname + OFFSET);
		return;
	}
	if (TST_RET == 0) {
		tst_res(TFAIL, "setxattr(2) on %s passed unexpectedly",
				tc[i].fname + OFFSET);
		return;
	}
	if (tc[i].exp_err != TST_ERR) {
		tst_res(TFAIL | TTERRNO,
				"setxattr(2) on %s should have failed with %s",
				tc[i].fname + OFFSET,
				tst_strerrno(tc[i].exp_err));
		return;
	}
	if (tc[i].needskeyset)
		removexattr(tc[i].fname, tc[i].key);
	tst_res(TPASS | TTERRNO, "setxattr(2) on %s failed",
			tc[i].fname + OFFSET);
}

void setup(void)
{
	dev_t dev = makedev(1, 3);
	touch(FILENAME, 0644, NULL);
	mkdir(DIRNAME, 0644);
	symlink(FILENAME, SYMLINK);
	mknod(FIFO, S_IFIFO | 0777, 0);
	mknod(CHR, S_IFCHR | 0777, dev);
	mknod(BLK, S_IFBLK | 0777, 0);
	mknod(SOCK, S_IFSOCK | 0777, 0);
}

