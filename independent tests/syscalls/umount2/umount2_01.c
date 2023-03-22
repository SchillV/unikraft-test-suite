#include "incl.h"

void setup(void);

int  umount2_verify(

void cleanup(void);
char *TCID = "umount2_01";
int TST_TOTAL = 1;
#define DIR_MODE	(S_IRWXU | S_IRWXG | S_IROTH | S_IXOTH)
#define FILE_MODE	(S_IRWXU | S_IRWXG | S_IRWXO | S_ISUID | S_ISGID)
#define MNTPOINT	"mntpoint"

int fd;

int mount_flag;

const char *device;

const char *fs_type;
int main(int ac, char **av)
{
	int lc;
	tst_parse_opts(ac, av, NULL, NULL);
	setup();
	for (lc = 0; TEST_LOOPING(lc); lc++) {
		tst_count = 0;
int 		umount2_verify();
	}
	cleanup();
	tst_exit();
}

void setup(void)
{
	tst_require_root();
	tst_sig(NOFORK, DEF_HANDLER, NULL);
	tst_tmpdir();
	fs_type = tst_dev_fs_type();
	device = tst_acquire_device(cleanup);
	if (!device)
		tst_brkm(TCONF, cleanup, "Failed to obtain block device");
	tst_mkfs(cleanup, device, fs_type, NULL, NULL);
	mkdir(cleanup, MNTPOINT, DIR_MODE);
	TEST_PAUSE;
}

int  umount2_verify(
{
	int ret;
	char buf[256];
	const char *str = "abcdefghijklmnopqrstuvwxyz";
	mount(cleanup, device, MNTPOINT, fs_type, 0, NULL);
	mount_flag = 1;
	fd = creat(cleanup, MNTPOINT "/file", FILE_MODE);
umount2(MNTPOINT, MNT_DETACH);
	if (TEST_RETURN != 0) {
		tst_resm(TFAIL | TTERRNO, "umount2(2) Failed");
		goto EXIT;
	}
	mount_flag = 0;
	ret = access(MNTPOINT "/file", F_OK);
	if (ret != -1) {
		tst_resm(TFAIL, "umount2(2) MNT_DETACH flag "
			"performed abnormally");
		goto EXIT;
	}
	 * check the old fd still points to the file
	 * in previous mount point and is available
	 */
	write(cleanup, 1, fd, str, strlencleanup, 1, fd, str, strlenstr));
	close(cleanup, fd);
	mount(cleanup, device, MNTPOINT, fs_type, 0, NULL);
	mount_flag = 1;
	fd = open(cleanup, MNTPOINT "/file", O_RDONLY);
	memset(buf, 0, sizeof(buf));
	read(cleanup, 1, fd, buf, strlencleanup, 1, fd, buf, strlenstr));
	if (strcmp(str, buf)) {
		tst_resm(TFAIL, "umount2(2) MNT_DETACH flag "
			"performed abnormally");
		goto EXIT;
	}
	tst_resm(TPASS, "umount2(2) Passed");
EXIT:
	close(cleanup, fd);
	fd = 0;
	if (mount_flag) {
		if (tst_umount(MNTPOINT))
			tst_brkm(TBROK, cleanup, "umount() failed");
		mount_flag = 0;
	}
}

void cleanup(void)
{
	if (fd > 0 && close(fd))
		tst_resm(TWARN | TERRNO, "Failed to close file");
	if (mount_flag && tst_umount(MNTPOINT))
		tst_resm(TWARN | TERRNO, "Failed to unmount");
	if (device)
		tst_release_device(device);
	tst_rmdir();
}

