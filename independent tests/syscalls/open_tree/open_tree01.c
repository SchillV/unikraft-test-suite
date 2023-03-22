#include "incl.h"
#define MNTPOINT	"mntpoint"
#define OT_MNTPOINT	"ot_mntpoint"
#define TCASE_ENTRY(_flags)	{.name = "Flag " #_flags, .flags = _flags}

struct tcase {
	char *name;
	unsigned int flags;
} tcases[] = {
	TCASE_ENTRY(OPEN_TREE_CLONE),
	TCASE_ENTRY(OPEN_TREE_CLOEXEC)
};

int dir_created;

void cleanup(void)
{
	if (dir_created)
		rmdir(OT_MNTPOINT);
}

void setup(void)
{
	fsopen_supported_by_kernel();
	mkdir(OT_MNTPOINT, 0777);
	dir_created = 1;
}

void run(unsigned int n)
{
	struct tcase *tc = &tcases[n];
	int otfd;
otfd = open_tree(AT_FDCWD, MNTPOINT, tc->flags | OPEN_TREE_CLONE);
	if (otfd == -1) {
		tst_res(TFAIL | TTERRNO, "open_tree() failed");
		return;
	}
move_mount(otfd, "", AT_FDCWD, OT_MNTPOIN;
			MOVE_MOUNT_F_EMPTY_PATH));
	close(otfd);
	if (TST_RET == -1) {
		tst_res(TFAIL | TTERRNO, "move_mount() failed");
		return;
	}
	if (tst_is_mounted_at_tmpdir(OT_MNTPOINT)) {
		umount(OT_MNTPOINT);
		tst_res(TPASS, "%s: open_tree() passed", tc->name);
	}
}

void main(){
	setup();
	cleanup();
}
