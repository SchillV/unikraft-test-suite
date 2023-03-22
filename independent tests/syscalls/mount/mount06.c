#include "incl.h"
#ifndef MS_MOVE
#define MS_MOVE	8192
#endif
#ifndef MS_PRIVATE
#define MS_PRIVATE	(1 << 18)
#endif
#define MNTPOINT_SRC	"mnt_src"
#define MNTPOINT_DES	"mnt_des"
#define LINELENGTH	256
#define DIR_MODE	(S_IRWXU | S_IRUSR | S_IXUSR | S_IRGRP | S_IXGRP)

int ismount(char *mntpoint);

void setup(void);

void cleanup(void);
char *TCID = "mount06";
int TST_TOTAL = 1;

const char *fs_type;

const char *device;

char path_name[PATH_MAX];

char mntpoint_src[PATH_MAX];

char mntpoint_des[PATH_MAX];

int mount_flag;
int main(int argc, char *argv[])
{
	int lc;
	tst_parse_opts(argc, argv, NULL, NULL);
	setup();
	for (lc = 0; TEST_LOOPING(lc); lc++) {
		tst_count = 0;
		mount(cleanup, device, mntpoint_src, fs_type, 0, NULL);
mount(mntpoint_src, mntpoint_des, fs_type, MS_MOVE, NULL);
		if (TEST_RETURN != 0) {
			tst_resm(TFAIL | TTERRNO, "mount(2) failed");
		} else {
			if (!ismount(mntpoint_src) && ismount(mntpoint_des))
				tst_resm(TPASS, "move mount is ok");
			else
				tst_resm(TFAIL, "move mount does not work");
tst_umount(mntpoint_des);
			if (TEST_RETURN != 0)
				tst_brkm(TBROK | TTERRNO, cleanup,
					 "umount(2) failed");
		}
	}
	cleanup();
	tst_exit();
}
int ismount(char *mntpoint)
{
	int ret = 0;
	FILE *file;
	char line[LINELENGTH];
	file = fopen("/proc/mounts", "r");
	if (file == NULL)
		tst_brkm(TFAIL | TERRNO, NULL, "Open /proc/mounts failed");
	while (fgets(line, LINELENGTH, file) != NULL) {
		if (strstr(line, mntpoint) != NULL) {
			ret = 1;
			break;
		}
	}
	fclose(file);
	return ret;
}

void setup(void)
{
	tst_require_root();
	tst_sig(NOFORK, DEF_HANDLER, cleanup);
	tst_tmpdir();
	fs_type = tst_dev_fs_type();
	device = tst_acquire_device(cleanup);
	if (!device)
		tst_brkm(TCONF, cleanup, "Failed to obtain block device");
	tst_mkfs(cleanup, device, fs_type, NULL, NULL);
	if (getcwd(path_name, sizeof(path_name)) == NULL)
		tst_brkm(TBROK, cleanup, "getcwd failed");
	 * Turn current dir into a private mount point being a parent
	 * mount which is required by move mount.
	 */
	mount(cleanup, path_name, path_name, "none", MS_BIND, NULL);
	mount_flag = 1;
	mount(cleanup, "none", path_name, "none", MS_PRIVATE, NULL);
	snprintf(mntpoint_src, PATH_MAX, "%s/%s", path_name, MNTPOINT_SRC);
	snprintf(mntpoint_des, PATH_MAX, "%s/%s", path_name, MNTPOINT_DES);
	mkdir(cleanup, mntpoint_src, DIR_MODE);
	mkdir(cleanup, mntpoint_des, DIR_MODE);
	TEST_PAUSE;
}

void cleanup(void)
{
	if (mount_flag && tst_umount(path_name) != 0)
		tst_resm(TWARN | TERRNO, "umount(2) %s failed", path_name);
	if (device)
		tst_release_device(device);
	tst_rmdir();
}

