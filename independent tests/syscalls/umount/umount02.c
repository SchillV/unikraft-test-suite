#include "incl.h"
#define MNTPOINT	"mntpoint"

char long_path[PATH_MAX + 2];

int mount_flag;

int fd;

struct tcase {
	const char *err_desc;
	const char *mntpoint;
	int exp_errno;
} tcases[] = {
	{"Already mounted/busy", MNTPOINT, EBUSY},
	{"Invalid address", NULL, EFAULT},
	{"Directory not found", "nonexistent", ENOENT},
	{"Invalid  device", "./", EINVAL},
	{"Pathname too long", long_path, ENAMETOOLONG}
};

int  verify_umount(unsigned int n)
{
	struct tcase *tc = &tcases[n];
umount(tc->mntpoint);
	if (TST_RET != -1) {
		tst_res(TFAIL, "umount() succeeds unexpectedly");
		return;
	}
	if (tc->exp_errno != TST_ERR) {
		tst_res(TFAIL | TTERRNO, "umount() should fail with %s",
			tst_strerrno(tc->exp_errno));
		return;
	}
	tst_res(TPASS | TTERRNO, "umount() fails as expected: %s",
		tc->err_desc);
}

void setup(void)
{
	memset(long_path, 'a', PATH_MAX + 1);
	mkdir(MNTPOINT, 0775);
	mount(tst_device->dev, MNTPOINT, tst_device->fs_type, 0, NULL);
	mount_flag = 1;
	fd = creat(MNTPOINT "/file", 0777);
}

void cleanup(void)
{
	if (fd > 0)
		close(fd);
	if (mount_flag)
		tst_umount(MNTPOINT);
}

void main(){
	setup();
	cleanup();
}
