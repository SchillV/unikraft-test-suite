#include "incl.h"
#define FOO_SYMLINK "foo_symlink"

struct open_how *how;

struct tcase {
	const char *name;
	const char *pathname;
	uint64_t resolve;
	int exp_errno;
} tcases[] = {
	{"open /proc/version", "/proc/version", 0, 0},
	{"open magiclinks", "/proc/self/exe", 0, 0},
	{"open symlinks", FOO_SYMLINK, 0, 0},
	{"resolve-no-xdev", "/proc/version", RESOLVE_NO_XDEV, EXDEV},
	{"resolve-no-magiclinks", "/proc/self/exe", RESOLVE_NO_MAGICLINKS, ELOOP},
	{"resolve-no-symlinks", FOO_SYMLINK, RESOLVE_NO_SYMLINKS, ELOOP},
	{"resolve-beneath", "/proc/version", RESOLVE_BENEATH, EXDEV},
	{"resolve-beneath", "../foo", RESOLVE_BENEATH, EXDEV},
	{"resolve-no-in-root", "/proc/version", RESOLVE_IN_ROOT, ENOENT},
};

void setup(void)
{
	openat2_supported_by_kernel();
	symlink("/proc/version", FOO_SYMLINK);
}

void run(unsigned int n)
{
	struct tcase *tc = &tcases[n];
	how->flags = O_RDONLY | O_CREAT;
	how->mode = S_IRUSR;
	how->resolve = tc->resolve;
openat2(AT_FDCWD, tc->pathname, how, sizeof(*how));
	if (!tc->exp_errno) {
		if (TST_RET < 0) {
			tst_res(TFAIL | TTERRNO, "%s: openat2() failed",
				tc->name);
			return;
		}
		close(TST_RET);
		tst_res(TPASS, "%s: openat2() passed", tc->name);
	} else {
		if (TST_RET >= 0) {
			close(TST_RET);
			tst_res(TFAIL, "%s: openat2() passed unexpectedly",
				tc->name);
			return;
		}
		if (tc->exp_errno != TST_ERR) {
			tst_res(TFAIL | TTERRNO, "%s: openat2() should fail with %s",
				tc->name, tst_strerrno(tc->exp_errno));
			return;
		}
		tst_res(TPASS | TTERRNO, "%s: openat2() failed as expected",
			tc->name);
	}
}

void main(){
	setup();
	cleanup();
}
