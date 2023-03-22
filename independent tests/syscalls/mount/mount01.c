#include "incl.h"

void setup(void);

void cleanup(void);
char *TCID = "mount01";
int TST_TOTAL = 1;
#define DIR_MODE (S_IRWXU | S_IRUSR | S_IXUSR | S_IRGRP | S_IXGRP)
#define MNTPOINT "mntpoint"

const char *device;

const char *fs_type;
int main(int ac, char **av)
{
	int lc;
	tst_parse_opts(ac, av, NULL, NULL);
	setup();
	for (lc = 0; TEST_LOOPING(lc); lc++) {
		tst_count = 0;
mount(device, MNTPOINT, fs_type, 0, NULL);
		if (TEST_RETURN != 0) {
			tst_resm(TFAIL | TTERRNO, "mount(2) failed");
		} else {
			tst_resm(TPASS, "mount(2) passed ");
tst_umount(MNTPOINT);
			if (TEST_RETURN != 0) {
				tst_brkm(TBROK | TTERRNO, cleanup,
					 "umount(2) failed");
			}
		}
	}
	cleanup();
	tst_exit();
}

void setup(void)
{
	tst_sig(NOFORK, DEF_HANDLER, cleanup);
	tst_require_root();
	tst_tmpdir();
	fs_type = tst_dev_fs_type();
	device = tst_acquire_device(cleanup);
	if (!device)
		tst_brkm(TCONF, cleanup, "Failed to obtain block device");
	tst_mkfs(cleanup, device, fs_type, NULL, NULL);
	mkdir(cleanup, MNTPOINT, DIR_MODE);
	TEST_PAUSE;
}

void cleanup(void)
{
	if (device)
		tst_release_device(device);
	tst_rmdir();
}

