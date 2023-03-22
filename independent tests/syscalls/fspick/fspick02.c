#include "incl.h"
#define MNTPOINT		"mntpoint"

struct tcase {
	char *name;
	int dirfd;
	const char *pathname;
	unsigned int flags;
	int exp_errno;
} tcases[] = {
	{"invalid-fd", -1, MNTPOINT, FSPICK_NO_AUTOMOUNT | FSPICK_CLOEXEC, EBADF},
	{"invalid-path", AT_FDCWD, "invalid", FSPICK_NO_AUTOMOUNT | FSPICK_CLOEXEC, ENOENT},
	{"invalid-flags", AT_FDCWD, MNTPOINT, 0x10, EINVAL},
};

void run(unsigned int n)
{
	struct tcase *tc = &tcases[n];
fspick(tc->dirfd, tc->pathname, tc->flags);
	if (TST_RET != -1) {
		close(TST_RET);
		tst_res(TFAIL, "%s: fspick() succeeded unexpectedly (index: %d)",
			tc->name, n);
		return;
	}
	if (tc->exp_errno != TST_ERR) {
		tst_res(TFAIL | TTERRNO, "%s: fspick() should fail with %s",
			tc->name, tst_strerrno(tc->exp_errno));
		return;
	}
	tst_res(TPASS | TTERRNO, "%s: fspick() failed as expected", tc->name);
}

void main(){
	setup();
	cleanup();
}
