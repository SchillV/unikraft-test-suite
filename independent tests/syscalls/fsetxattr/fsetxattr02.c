#include "incl.h"
#ifdef HAVE_SYS_XATTR_H
# include <sys/xattr.h>
#endif
#ifdef HAVE_SYS_XATTR_H
#define XATTR_TEST_KEY "user.testkey"
#define XATTR_TEST_VALUE "this is a test value"
#define XATTR_TEST_VALUE_SIZE 20
#define MNTPOINT "mntpoint"
#define OFFSET    11
#define FILENAME "fsetxattr02testfile"
#define DIRNAME  "fsetxattr02testdir"
#define SYMLINK  "fsetxattr02symlink"
#define FIFO     MNTPOINT"/fsetxattr02fifo"
#define CHR      MNTPOINT"/fsetxattr02chr"
#define BLK      MNTPOINT"/fsetxattr02blk"
#define SOCK     "fsetxattr02sock"
struct test_case {
	char *fname;
	int fd;
	int fflags;
	char *key;
	char *value;
	size_t size;
	int flags;
	int exp_err;
	int issocket;
	int needskeyset;
};

struct test_case tc[] = {
	 .fname = FILENAME,
	 .fflags = O_RDONLY,
	 .key = XATTR_TEST_KEY,
	 .value = XATTR_TEST_VALUE,
	 .size = XATTR_TEST_VALUE_SIZE,
	 .flags = XATTR_CREATE,
	 .exp_err = 0,
	 },
	 .fname = DIRNAME,
	 .fflags = O_RDONLY,
	 .key = XATTR_TEST_KEY,
	 .value = XATTR_TEST_VALUE,
	 .size = XATTR_TEST_VALUE_SIZE,
	 .flags = XATTR_CREATE,
	 .exp_err = 0,
	 },
	 .fname = SYMLINK,
	 .fflags = O_RDONLY,
	 .key = XATTR_TEST_KEY,
	 .value = XATTR_TEST_VALUE,
	 .size = XATTR_TEST_VALUE_SIZE,
	 .flags = XATTR_CREATE,
	 .exp_err = EEXIST,
	 .needskeyset = 1,
	 },
	 .fname = FIFO,
	 .fflags = (O_RDONLY | O_NONBLOCK),
	 .key = XATTR_TEST_KEY,
	 .value = XATTR_TEST_VALUE,
	 .size = XATTR_TEST_VALUE_SIZE,
	 .flags = XATTR_CREATE,
	 .exp_err = EPERM,
	 },
	 .fname = CHR,
	 .fflags = O_RDONLY,
	 .key = XATTR_TEST_KEY,
	 .value = XATTR_TEST_VALUE,
	 .size = XATTR_TEST_VALUE_SIZE,
	 .flags = XATTR_CREATE,
	 .exp_err = EPERM,
	 },
	 .fname = BLK,
	 .fflags = O_RDONLY,
	 .key = XATTR_TEST_KEY,
	 .value = XATTR_TEST_VALUE,
	 .size = XATTR_TEST_VALUE_SIZE,
	 .flags = XATTR_CREATE,
	 .exp_err = EPERM,
	 },
	 .fname = SOCK,
	 .fflags = O_RDONLY,
	 .key = XATTR_TEST_KEY,
	 .value = XATTR_TEST_VALUE,
	 .size = XATTR_TEST_VALUE_SIZE,
	 .flags = XATTR_CREATE,
	 .exp_err = EPERM,
	 .issocket = 1,
	 },
};

int  verify_fsetxattr(unsigned int i)
{
	const char *fname = strstr(tc[i].fname, "fsetxattr02") + OFFSET;
	if (tc[i].needskeyset) {
		fsetxattr(tc[i].fd, tc[i].key, tc[i].value, tc[i].size,
				XATTR_CREATE);
	}
fsetxattr(tc[i].fd, tc[i].key, tc[i].value, tc[i].siz;
			tc[i].flags));
	if (TST_RET == -1 && TST_ERR == EOPNOTSUPP)
		tst_brk(TCONF, "fsetxattr(2) not supported");
	if (!tc[i].exp_err) {
		if (TST_RET) {
			tst_res(TFAIL | TTERRNO,
				"fsetxattr(2) on %s failed with %li",
				fname, TST_RET);
			return;
		}
		fremovexattr(tc[i].fd, tc[i].key);
		tst_res(TPASS, "fsetxattr(2) on %s passed", fname);
		return;
	}
	if (TST_RET == 0) {
		tst_res(TFAIL, "fsetxattr(2) on %s passed unexpectedly", fname);
		return;
	}
	if (tc[i].exp_err != TST_ERR) {
		tst_res(TFAIL | TTERRNO,
				"fsetxattr(2) on %s should have failed with %s",
				fname, tst_strerrno(tc[i].exp_err));
		return;
	}
	if (tc[i].needskeyset)
		fremovexattr(tc[i].fd, tc[i].key);
	tst_res(TPASS | TTERRNO, "fsetxattr(2) on %s failed", fname);
}

void setup(void)
{
	size_t i = 0;
	struct sockaddr_un sun;
	dev_t dev = makedev(1, 3);
	touch(FILENAME, 0644, NULL);
	mkdir(DIRNAME, 0644);
	symlink(FILENAME, SYMLINK);
	mknod(FIFO, S_IFIFO | 0777, 0);
	mknod(CHR, S_IFCHR | 0777, dev);
	mknod(BLK, S_IFBLK | 0777, dev);
	for (i = 0; i < ARRAY_SIZE(tc); i++) {
		if (!tc[i].issocket) {
			tc[i].fd = open(tc[i].fname, tc[i].fflags);
			continue;
		}
		 * sockets, mknod() isn't enough to test fsetxattr(2).
		 * we have to get a real unix socket in order for open()
		 * to get a file desc.
		 */
		tc[i].fd = socket(AF_UNIX, SOCK_STREAM, 0);
		memset(&sun, 0, sizeof(struct sockaddr_un));
		sun.sun_family = AF_UNIX;
		strncpy(sun.sun_path, tc[i].fname, sizeof(sun.sun_path) - 1);
		bind(tc[i].fd, tc[i].fd, const struct sockaddr *) &sun,
				sizeof(struct sockaddr_un));
	}
}

void cleanup(void)
{
	size_t i = 0;
	for (i = 0; i < ARRAY_SIZE(tc); i++) {
		if (tc[i].fd > 0)
			close(tc[i].fd);
	}
}

