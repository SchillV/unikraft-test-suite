#include "incl.h"
#define MNTPOINT	"mntpoint"

struct tcase {
	char *name;
	int dirfd;
	const char *pathname;
	unsigned int flags;
	int exp_errno;
} tcases[] = {
	{"invalid-fd", -1, MNTPOINT, OPEN_TREE_CLONE, EBADF},
	{"invalid-path", AT_FDCWD, "invalid", OPEN_TREE_CLONE, ENOENT},
	{"invalid-flags", AT_FDCWD, MNTPOINT, 0xFFFFFFFF, EINVAL},
};

void run(unsigned int n)
{
	struct tcase *tc = &tcases[n];
open_tree(tc->dirfd, tc->pathname, tc->flags);
	if (TST_RET != -1) {
		close(TST_RET);
		tst_res(TFAIL, "%s: open_tree() succeeded unexpectedly (index: %d)",
			tc->name, n);
		return;
	}
	if (tc->exp_errno != TST_ERR) {
		tst_res(TFAIL | TTERRNO, "%s: open_tree() should fail with %s",
			tc->name, tst_strerrno(tc->exp_errno));
		return;
	}
	tst_res(TPASS | TTERRNO, "%s: open_tree() failed as expected",
		tc->name);
}

void main(){
	setup();
	cleanup();
}
