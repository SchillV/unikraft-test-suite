#include "incl.h"

void help(void);

void setup(void);

void cleanup(void);
char *TCID = "mount05";
int TST_TOTAL = 1;
#define DIR_MODE	(S_IRWXU | S_IRUSR | S_IXUSR | S_IRGRP | S_IXGRP)

int dflag;

char *fstype = "ext2";

char *device;

const char file_src[] = "mnt_src/tstfile";

const char file_des[] = "mnt_des/tstfile";

const char mntpoint_src[] = "mnt_src";

const char mntpoint_des[] = "mnt_des";

option_t options[] = {
	{"T:", NULL, &fstype},
	{"D:", &dflag, &device},
	{NULL, NULL, NULL},
};
int main(int argc, char *argv[])
{
	int lc;
	tst_parse_opts(argc, argv, options, &help);
	setup();
	for (lc = 0; TEST_LOOPING(lc); lc++) {
		tst_count = 0;
mount(mntpoint_src, mntpoint_des, fstype, MS_BIND, NULL);
		if (TEST_RETURN != 0) {
			tst_resm(TFAIL | TTERRNO, "mount(2) failed");
		} else {
			if (open(file_des, O_CREAT | O_EXCL, S_IRWXU) == -1 &&
			    errno == EEXIST)
				tst_resm(TPASS, "bind mount is ok");
			else
				tst_resm(TFAIL, "file %s is not available",
					 file_des);
tst_umount(mntpoint_des);
			if (TEST_RETURN != 0)
				tst_brkm(TBROK | TTERRNO, cleanup,
					 "umount(2) failed");
		}
	}
	cleanup();
	tst_exit();
}
void setup(void)
{
	tst_require_root();
	tst_sig(NOFORK, DEF_HANDLER, cleanup);
	tst_tmpdir();
	mkdir(cleanup, mntpoint_src, DIR_MODE);
	mkdir(cleanup, mntpoint_des, DIR_MODE);
	if (dflag) {
		tst_mkfs(NULL, device, fstype, NULL, NULL);
		mount(cleanup, device, mntpoint_src, fstype, 0, NULL);
	}
	fprintf(cleanup, file_src, "TEST FILE");
	TEST_PAUSE;
}
void cleanup(void)
{
	if (dflag)
		if (tst_umount(mntpoint_src) != 0)
			tst_brkm(TBROK | TTERRNO, NULL, "umount(2) failed");
	tst_rmdir();
}
void help(void)
{
	printf("-T type	  : specifies the type of filesystem to be mounted. "
	       "Default ext2.\n");
	printf("-D device : device used for mounting.\n");
}

