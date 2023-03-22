#include "incl.h"
#ifdef HAVE_SYS_XATTR_H
# include <sys/xattr.h>
#endif
#ifdef HAVE_SYS_XATTR_H
#define XATTR_NAME_MAX 255
#define XATTR_NAME_LEN (XATTR_NAME_MAX + 2)
#define XATTR_SIZE_MAX 65536
#define XATTR_TEST_KEY "user.testkey"
#define XATTR_TEST_VALUE "this is a test value"
#define XATTR_TEST_VALUE_SIZE 20
#define MNTPOINT "mntpoint"
#define FNAME MNTPOINT"/fsetxattr01testfile"

int fd = -1;

char long_key[XATTR_NAME_LEN];

char *long_value;

char *xattr_value = XATTR_TEST_VALUE;
struct test_case {
	char *key;
	char **value;
	size_t size;
	int flags;
	int exp_err;
	int keyneeded;
};
struct test_case tc[] = {
	 .key = XATTR_TEST_KEY,
	 .value = &xattr_value,
	 .size = XATTR_TEST_VALUE_SIZE,
	 .flags = ~0,
	 .exp_err = EINVAL,
	 },
	 .key = XATTR_TEST_KEY,
	 .value = &xattr_value,
	 .size = XATTR_TEST_VALUE_SIZE,
	 .flags = XATTR_REPLACE,
	 .exp_err = ENODATA,
	 },
	 .key = long_key,
	 .value = &xattr_value,
	 .size = XATTR_TEST_VALUE_SIZE,
	 .flags = XATTR_CREATE,
	 .exp_err = ERANGE,
	 },
	 .key = XATTR_TEST_KEY,
	 .value = &long_value,
	 .size = XATTR_SIZE_MAX + 1,
	 .flags = XATTR_CREATE,
	 .exp_err = E2BIG,
	 },
	 .key = XATTR_TEST_KEY,
	 .value = &xattr_value,
	 .size = 0,
	 .flags = XATTR_CREATE,
	 .exp_err = 0,
	 },
	 .key = XATTR_TEST_KEY,
	 .value = &xattr_value,
	 .size = XATTR_TEST_VALUE_SIZE,
	 .flags = XATTR_CREATE,
	 .exp_err = EEXIST,
	 .keyneeded = 1,
	 },
	 .key = XATTR_TEST_KEY,
	 .value = &xattr_value,
	 .size = XATTR_TEST_VALUE_SIZE,
	 .flags = XATTR_REPLACE,
	 .exp_err = 0,
	 .keyneeded = 1,
	},
	 .key = "",
	 .value = &xattr_value,
	 .size = XATTR_TEST_VALUE_SIZE,
	 .flags = XATTR_CREATE,
	 .exp_err = ERANGE,
	},
	 .value = &xattr_value,
	 .size = XATTR_TEST_VALUE_SIZE,
	 .flags = XATTR_CREATE,
	 .exp_err = EFAULT,
	},
};

int  verify_fsetxattr(unsigned int i)
{
	if (tc[i].keyneeded) {
		fsetxattr(fd, tc[i].key, tc[i].value, tc[i].size,
				XATTR_CREATE);
	}
fsetxattr(fd, tc[i].key, *tc[i].value, tc[i].size, tc[i].flags);
	if (TST_RET == -1 && TST_ERR == EOPNOTSUPP)
		tst_brk(TCONF, "fsetxattr(2) not supported");
	if (!tc[i].exp_err) {
		if (TST_RET) {
			tst_res(TFAIL | TTERRNO,
				"fsetxattr(2) failed with %li", TST_RET);
			return;
		}
		fremovexattr(fd, tc[i].key);
		tst_res(TPASS, "fsetxattr(2) passed");
		return;
	}
	if (TST_RET == 0) {
		tst_res(TFAIL, "fsetxattr(2) passed unexpectedly");
		return;
	}
	if (tc[i].exp_err != TST_ERR) {
		tst_res(TFAIL | TTERRNO, "fsetxattr(2) should fail with %s",
			tst_strerrno(tc[i].exp_err));
		return;
	}
	if (tc[i].keyneeded)
		fremovexattr(fd, tc[i].key);
	tst_res(TPASS | TTERRNO, "fsetxattr(2) failed");
}

void cleanup(void)
{
	if (fd > 0)
		close(fd);
}

void setup(void)
{
	size_t i = 0;
	snprintf(long_key, 6, "%s", "user.");
	memset(long_key + 5, 'k', XATTR_NAME_LEN - 5);
	long_key[XATTR_NAME_LEN - 1] = '\0';
	long_value = malloc(XATTR_SIZE_MAX + 2);
	memset(long_value, 'v', XATTR_SIZE_MAX + 2);
	long_value[XATTR_SIZE_MAX + 1] = '\0';
	touch(FNAME, 0644, NULL);
	fd = open(FNAME, O_RDONLY);
	for (i = 0; i < ARRAY_SIZE(tc); i++) {
		if (!tc[i].key)
			tc[i].key = tst_get_bad_addr(cleanup);
	}
}

void main(){
	setup();
	cleanup();
}
