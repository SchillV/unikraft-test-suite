#include "incl.h"

void setup(void);

void cleanup(void);
char *TCID = "mount04";
#define DIR_MODE	S_IRWXU | S_IRUSR | S_IXUSR | S_IRGRP | S_IXGRP

char *mntpoint = "mntpoint";

const char *fs_type;

const char *device;
int TST_TOTAL = 1;

int  verify_mount(
{
mount(device, mntpoint, fs_type, 0, NULL);
	if (TEST_RETURN == -1) {
		if (TEST_ERRNO == EPERM)
			tst_resm(TPASS | TTERRNO, "mount() failed expectedly");
		else
			tst_resm(TFAIL | TTERRNO,
			         "mount() was expected to fail with EPERM");
		return;
	}
	if (TEST_RETURN == 0) {
		tst_resm(TFAIL, "mount() succeeded unexpectedly");
		if (tst_umount(mntpoint))
			tst_brkm(TBROK, cleanup, "umount() failed");
		return;
	}
	tst_resm(TFAIL | TTERRNO, "mount() returned %li", TEST_RETURN);
}
int main(int ac, char **av)
{
	int lc;
	tst_parse_opts(ac, av, NULL, NULL);
	setup();
	for (lc = 0; TEST_LOOPING(lc); lc++) {
		tst_count = 0;
int 		verify_mount();
	}
	cleanup();
	tst_exit();
}

void setup(void)
{
	char nobody_uid[] = "nobody";
	struct passwd *ltpuser;
	tst_sig(FORK, DEF_HANDLER, cleanup);
	tst_require_root();
	tst_tmpdir();
	fs_type = tst_dev_fs_type();
	device = tst_acquire_device(cleanup);
	if (!device)
		tst_brkm(TCONF, cleanup, "Failed to obtain block device");
	tst_mkfs(cleanup, device, fs_type, NULL, NULL);
	ltpuser = getpwnam(cleanup, nobody_uid);
	seteuid(cleanup, ltpuser->pw_uid);
	mkdir(cleanup, mntpoint, DIR_MODE);
	TEST_PAUSE;
}

void cleanup(void)
{
	if (seteuid(0))
		tst_resm(TWARN | TERRNO, "seteuid(0) failed");
	if (device)
		tst_release_device(device);
	tst_rmdir();
}

