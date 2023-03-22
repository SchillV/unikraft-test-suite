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
#define FILENAME "fgetxattr02testfile"
#define DIRNAME  "fgetxattr02testdir"
#define SYMLINK  "fgetxattr02symlink"
#define SYMLINKF "fgetxattr02symlinkfile"
#define FIFO     MNTPOINT"/fgetxattr02fifo"
#define CHR      MNTPOINT"/fgetxattr02chr"
#define BLK      MNTPOINT"/fgetxattr02blk"
#define SOCK     "fgetxattr02sock"
struct test_case {
	char *fname;
	int fd;
	int fflags;
	char *key;
	char *value;
	size_t size;
	char *ret_value;
	int flags;
	int exp_err;
	int exp_ret;
	int issocket;
};

struct test_case tc[] = {
	 .fname = FILENAME,
	 .fflags = O_RDONLY,
	 .key = XATTR_TEST_KEY,
	 .value = XATTR_TEST_VALUE,
	 .size = XATTR_TEST_VALUE_SIZE,
	 .ret_value = NULL,
	 .flags = XATTR_CREATE,
	 .exp_err = 0,
	 .exp_ret = XATTR_TEST_VALUE_SIZE,
	 },
	 .fname = DIRNAME,
	 .fflags = O_RDONLY,
	 .key = XATTR_TEST_KEY,
	 .value = XATTR_TEST_VALUE,
	 .size = XATTR_TEST_VALUE_SIZE,
	 .ret_value = NULL,
	 .flags = XATTR_CREATE,
	 .exp_err = 0,
	 .exp_ret = XATTR_TEST_VALUE_SIZE,
	 },
	 .fname = SYMLINK,
	 .fflags = O_RDONLY,
	 .key = XATTR_TEST_KEY,
	 .value = XATTR_TEST_VALUE,
	 .size = XATTR_TEST_VALUE_SIZE,
	 .ret_value = NULL,
	 .flags = XATTR_CREATE,
	 .exp_err = 0,
	 .exp_ret = XATTR_TEST_VALUE_SIZE,
	 },
	 .fname = FIFO,
	 .fflags = (O_RDONLY | O_NONBLOCK),
	 .key = XATTR_TEST_KEY,
	 .value = XATTR_TEST_VALUE,
	 .size = XATTR_TEST_VALUE_SIZE,
	 .flags = XATTR_CREATE,
	 .exp_err = ENODATA,
	 .exp_ret = -1,
	 },
	 .fname = CHR,
	 .fflags = O_RDONLY,
	 .key = XATTR_TEST_KEY,
	 .value = XATTR_TEST_VALUE,
	 .size = XATTR_TEST_VALUE_SIZE,
	 .ret_value = NULL,
	 .flags = XATTR_CREATE,
	 .exp_err = ENODATA,
	 .exp_ret = -1,
	 },
	 .fname = BLK,
	 .fflags = O_RDONLY,
	 .key = XATTR_TEST_KEY,
	 .value = XATTR_TEST_VALUE,
	 .size = XATTR_TEST_VALUE_SIZE,
	 .ret_value = NULL,
	 .flags = XATTR_CREATE,
	 .exp_err = ENODATA,
	 .exp_ret = -1,
	 },
	 .fname = SOCK,
	 .fflags = O_RDONLY,
	 .key = XATTR_TEST_KEY,
	 .value = XATTR_TEST_VALUE,
	 .size = XATTR_TEST_VALUE_SIZE,
	 .ret_value = NULL,
	 .flags = XATTR_CREATE,
	 .exp_err = ENODATA,
	 .exp_ret = -1,
	 .issocket = 1,
	 },
};

int  verify_fgetxattr(unsigned int i)
{
	const char *fname = strstr(tc[i].fname, "fgetxattr02") + OFFSET;
fgetxattr(tc[i].fd, tc[i].key, tc[i].ret_value, tc[i].size);
	if (TST_RET == -1 && TST_ERR == EOPNOTSUPP)
		tst_brk(TCONF, "fgetxattr(2) not supported");
	if (TST_RET >= 0) {
		if (tc[i].exp_ret == TST_RET) {
			tst_res(TPASS, "fgetxattr(2) on %s passed",
					fname);
		} else {
			tst_res(TFAIL,
				"fgetxattr(2) on %s passed unexpectedly %ld",
				fname, TST_RET);
		}
		if (strncmp(tc[i].ret_value, XATTR_TEST_VALUE,
				XATTR_TEST_VALUE_SIZE)) {
			tst_res(TFAIL, "wrong value, expect \"%s\" got \"%s\"",
					 XATTR_TEST_VALUE, tc[i].ret_value);
		}
		tst_res(TPASS, "fgetxattr(2) on %s got the right value",
				fname);
	}
	if (tc[i].exp_err == TST_ERR) {
		tst_res(TPASS | TTERRNO, "fgetxattr(2) on %s passed",
				fname);
		return;
	}
	tst_res(TFAIL | TTERRNO, "fgetxattr(2) failed on %s", fname);
}

void setup(void)
{
	size_t i = 0;
	struct sockaddr_un sun;
	dev_t chr_dev = makedev(1, 3);
	dev_t blk_dev = makedev(7, 3);
	touch(FILENAME, 0644, NULL);
	touch(SYMLINKF, 0644, NULL);
	mkdir(DIRNAME, 0644);
	symlink(SYMLINKF, SYMLINK);
	mknod(FIFO, S_IFIFO | 0777, 0);
	mknod(CHR, S_IFCHR | 0777, chr_dev);
	mknod(BLK, S_IFBLK | 0777, blk_dev);
	for (i = 0; i < ARRAY_SIZE(tc); i++) {
		tc[i].ret_value = malloc(tc[i].size);
		memset(tc[i].ret_value, 0, tc[i].size);
		if (tc[i].issocket) {
			 * sockets, mknod(2) isn't enough to test fgetxattr(2).
			 * we have to get a real unix socket in order for
			 * open(2) to get a file desc.
			 */
			tc[i].fd = socket(AF_UNIX, SOCK_STREAM, 0);
			memset(&sun, 0, sizeof(struct sockaddr_un));
			sun.sun_family = AF_UNIX;
			strncpy(sun.sun_path, tc[i].fname,
					sizeof(sun.sun_path) - 1);
			bind(tc[i].fd, tc[i].fd, const struct sockaddr *) &sun,
					sizeof(struct sockaddr_un));
		} else {
			tc[i].fd = open(tc[i].fname, tc[i].fflags);
		}
		if (tc[i].exp_ret >= 0) {
			fsetxattr(tc[i].fd, tc[i].key, tc[i].value,
					tc[i].size, tc[i].flags);
		}
	}
}

void cleanup(void)
{
	size_t i = 0;
	for (i = 0; i < ARRAY_SIZE(tc); i++) {
		free(tc[i].ret_value);
		if (tc[i].fd > 0)
			close(tc[i].fd);
	}
}

