#include "incl.h"
#ifdef HAVE_SYS_XATTR_H
# include <sys/xattr.h>
#endif
#ifdef HAVE_SYS_XATTR_H
#define XATTR_SIZE_MAX 65536
#define XATTR_TEST_KEY "user.testkey"
#define XATTR_TEST_VALUE "this is a test value"
#define XATTR_TEST_VALUE_SIZE 20
#define XATTR_TEST_INVALID_KEY "user.nosuchkey"
#define MNTPOINT "mntpoint"
#define FNAME MNTPOINT"/fgetxattr01testfile"

int fd = -1;
struct test_case {
	char *key;
	char *value;
	size_t size;
	int exp_ret;
	int exp_err;
};
struct test_case tc[] = {
	 .key = XATTR_TEST_INVALID_KEY,
	 .value = NULL,
	 .size = XATTR_SIZE_MAX,
	 .exp_ret = -1,
	 .exp_err = ENODATA,
	 },
	 .key = XATTR_TEST_KEY,
	 .value = NULL,
	 .size = 1,
	 .exp_ret = -1,
	 .exp_err = ERANGE,
	 },
	 .key = XATTR_TEST_KEY,
	 .value = NULL,
	 .size = XATTR_TEST_VALUE_SIZE,
	 .exp_ret = XATTR_TEST_VALUE_SIZE,
	 .exp_err = 0,
	 },
};

int  verify_fgetxattr(unsigned int i)
{
fgetxattr(fd, tc[i].key, tc[i].value, tc[i].size);
	if (TST_RET == -1 && TST_ERR == EOPNOTSUPP)
		tst_brk(TCONF, "fgetxattr(2) not supported");
	if (TST_RET >= 0) {
		if (tc[i].exp_ret == TST_RET)
			tst_res(TPASS, "fgetxattr(2) passed");
		else
			tst_res(TFAIL, "fgetxattr(2) passed unexpectedly");
		if (strncmp(tc[i].value, XATTR_TEST_VALUE,
				XATTR_TEST_VALUE_SIZE)) {
			tst_res(TFAIL, "wrong value, expect \"%s\" got \"%s\"",
					 XATTR_TEST_VALUE, tc[i].value);
		}
		tst_res(TPASS, "got the right value");
	}
	if (tc[i].exp_err == TST_ERR) {
		tst_res(TPASS | TTERRNO, "fgetxattr(2) passed");
		return;
	}
	tst_res(TFAIL | TTERRNO, "fgetxattr(2) failed");
}

void setup(void)
{
	size_t i = 0;
	touch(FNAME, 0644, NULL);
	fd = open(FNAME, O_RDONLY);
	for (i = 0; i < ARRAY_SIZE(tc); i++) {
		tc[i].value = malloc(tc[i].size);
		memset(tc[i].value, 0, tc[i].size);
	}
	fsetxattr(fd, XATTR_TEST_KEY, XATTR_TEST_VALUE,
			XATTR_TEST_VALUE_SIZE, XATTR_CREATE);
}

void cleanup(void)
{
	size_t i = 0;
	for (i = 0; i < ARRAY_SIZE(tc); i++)
		free(tc[i].value);
	if (fd > 0)
		close(fd);
}

void main(){
	setup();
	cleanup();
}
