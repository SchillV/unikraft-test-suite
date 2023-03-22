#include "incl.h"
#define MNTPOINT		"mntpoint"
#define TCASE_ENTRY(_flags)	{.name = "Flag " #_flags, .flags = _flags}

struct tcase {
	char *name;
	unsigned int flags;
} tcases[] = {
	TCASE_ENTRY(FSPICK_CLOEXEC),
	TCASE_ENTRY(FSPICK_SYMLINK_NOFOLLOW),
	TCASE_ENTRY(FSPICK_NO_AUTOMOUNT),
	TCASE_ENTRY(FSPICK_EMPTY_PATH),
};

void run(unsigned int n)
{
	struct tcase *tc = &tcases[n];
	int fspick_fd;
fspick_fd = fspick(AT_FDCWD, MNTPOINT, tc->flags);
	if (fspick_fd == -1) {
		tst_res(TFAIL | TTERRNO, "fspick() failed");
		return;
	}
fsconfig(fspick_fd, FSCONFIG_SET_STRING, "sync", "false", 0);
	if (TST_RET == -1) {
		tst_res(TFAIL | TTERRNO, "fsconfig(FSCONFIG_SET_STRING) failed");
		goto out;
	}
fsconfig(fspick_fd, FSCONFIG_SET_FLAG, "ro", NULL, 0);
	if (TST_RET == -1) {
		tst_res(TFAIL | TTERRNO, "fsconfig(FSCONFIG_SET_FLAG) failed");
		goto out;
	}
fsconfig(fspick_fd, FSCONFIG_CMD_RECONFIGURE, NULL, NULL, 0);
	if (TST_RET == -1) {
		tst_res(TFAIL | TTERRNO, "fsconfig(FSCONFIG_CMD_RECONFIGURE) failed");
		goto out;
	}
	tst_res(TPASS, "%s: fspick() passed", tc->name);
out:
	close(fspick_fd);
}

void main(){
	setup();
	cleanup();
}
