#include "incl.h"
const char *invalid_fs = "invalid";
const char *valid_fs;

struct tcase {
	char *name;
	const char **fs;
	unsigned int flags;
	int exp_errno;
} tcases[] = {
	{"invalid-fs", &invalid_fs, 0, ENODEV},
	{"invalid-flags", &valid_fs, 0x10, EINVAL},
};

void setup(void)
{
	fsopen_supported_by_kernel();
	valid_fs = tst_device->fs_type;
}

void run(unsigned int n)
{
	struct tcase *tc = &tcases[n];
fsopen(*tc->fs, tc->flags);
	if (TST_RET != -1) {
		close(TST_RET);
		tst_res(TFAIL, "%s: fsopen() succeeded unexpectedly (index: %d)",
			tc->name, n);
		return;
	}
	if (tc->exp_errno != TST_ERR) {
		tst_res(TFAIL | TTERRNO, "%s: fsopen() should fail with %s",
			tc->name, tst_strerrno(tc->exp_errno));
		return;
	}
	tst_res(TPASS | TTERRNO, "%s: fsopen() failed as expected", tc->name);
}

void main(){
	setup();
	cleanup();
}
