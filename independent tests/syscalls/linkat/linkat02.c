#include "incl.h"
#define _GNU_SOURCE
#define DIR_MODE	(S_IRUSR|S_IWUSR|S_IXUSR|S_IRGRP| \
			 S_IXGRP|S_IROTH|S_IXOTH)
#define TEST_FILE	"testfile"
#define TEST_EXIST	"testexist"
#define TEST_ELOOP	"testeloop"
#define TEST_EACCES	"./tmp/testeeacces"
#define TEST_EACCES2	"./tmp/testeeacces2"
#define TEST_EROFS	"mntpoint"
#define TEST_EROFS2	"mntpoint/testerofs2"
#define TEST_EMLINK	"emlink_dir/testfile0"
#define TEST_EMLINK2	"emlink_dir/testfile"
#define BASENAME	"mntpoint/basename"

char nametoolong[PATH_MAX+2];

const char *device;

int mount_flag;

int max_hardlinks;

const char *fs_type;

void setup(void);

void cleanup(void);

void setup_eacces(void);

void cleanup_eacces(void);

void setup_erofs(void);

struct test_struct {
	const char *oldfname;
	const char *newfname;
	int flags;
	int expected_errno;
	void (*setupfunc) (void);
	void (*cleanfunc) (void);
} test_cases[] = {
	{TEST_FILE, nametoolong, 0, ENAMETOOLONG, NULL, NULL},
	{nametoolong, TEST_FILE, 0, ENAMETOOLONG, NULL, NULL},
	{TEST_EXIST, TEST_EXIST, 0, EEXIST, NULL, NULL},
	{TEST_ELOOP, TEST_FILE, AT_SYMLINK_FOLLOW, ELOOP, NULL, NULL},
	{TEST_EACCES, TEST_EACCES2, 0, EACCES, setup_eacces, cleanup_eacces},
	{TEST_EROFS, TEST_EROFS2, 0, EROFS, setup_erofs, NULL},
	{TEST_EMLINK, TEST_EMLINK2, 0, EMLINK, NULL, NULL},
};
char *TCID = "linkat02";
int TST_TOTAL = ARRAY_SIZE(test_cases);

struct passwd *ltpuser;

int  linkat_verify(const struct test_struct *);
int main(int ac, char **av)
{
	int lc;
	int i;
	tst_parse_opts(ac, av, NULL, NULL);
	setup();
	for (lc = 0; TEST_LOOPING(lc); lc++) {
		tst_count = 0;
		for (i = 0; i < TST_TOTAL; i++)
int 			linkat_verify(&test_cases[i]);
	}
	cleanup();
	tst_exit();
}

int  linkat_verify(const struct test_struct *desc)
{
	if (desc->expected_errno == EMLINK && max_hardlinks == 0) {
		tst_resm(TCONF, "EMLINK test is not appropriate");
		return;
	}
	if (desc->setupfunc != NULL) {
		desc->setupfunc();
	}
tst_syscall(__NR_linkat, AT_FDCWD, desc->oldfnam;
			 AT_FDCWD, desc->newfname, desc->flags));
	if (desc->cleanfunc != NULL)
		desc->cleanfunc();
	if (TEST_RETURN != -1) {
		tst_resm(TFAIL,
			 "linkat(""AT_FDCWD"", %s, ""AT_FDCWD"", %s, %d) "
			 "succeeded unexpectedly", desc->oldfname,
			 desc->newfname, desc->flags);
		return;
	}
	if (TEST_ERRNO == desc->expected_errno) {
		tst_resm(TPASS | TTERRNO, "linkat failed as expected");
	} else {
		tst_resm(TFAIL | TTERRNO,
			 "linkat failed unexpectedly; expected: "
			 "%d - %s", desc->expected_errno,
			 strerror(desc->expected_errno));
	}
}

void setup(void)
{
	tst_require_root();
	tst_sig(NOFORK, DEF_HANDLER, cleanup);
	tst_tmpdir();
	fs_type = tst_dev_fs_type();
	device = tst_acquire_device(cleanup);
	if (!device)
		tst_brkm(TCONF, cleanup, "Failed to acquire device");
	TEST_PAUSE;
	ltpuser = getpwnam(cleanup, "nobody");
	touch(cleanup, TEST_FILE, 0644, NULL);
	memset(nametoolong, 'a', PATH_MAX+1);
	touch(cleanup, TEST_EXIST, 0644, NULL);
	symlink(cleanup, TEST_ELOOP, "test_file_eloop2");
	symlink(cleanup, "test_file_eloop2", TEST_ELOOP);
	mkdir(cleanup, "./tmp", DIR_MODE);
	touch(cleanup, TEST_EACCES, 0666, NULL);
	tst_mkfs(cleanup, device, fs_type, NULL, NULL);
	mkdir(cleanup, "mntpoint", DIR_MODE);
	mount(cleanup, device, "mntpoint", fs_type, 0, NULL);
	mount_flag = 1;
	max_hardlinks = tst_fs_fill_hardlinks(cleanup, "emlink_dir");
}

void setup_eacces(void)
{
	seteuid(cleanup, ltpuser->pw_uid);
}

void cleanup_eacces(void)
{
	seteuid(cleanup, 0);
}

void setup_erofs(void)
{
	mount(cleanup, device, "mntpoint", fs_type,
		   MS_REMOUNT | MS_RDONLY, NULL);
	mount_flag = 1;
}

void cleanup(void)
{
	if (mount_flag && tst_umount("mntpoint") < 0)
		tst_resm(TWARN | TERRNO, "umount device:%s failed", device);
	if (device)
		tst_release_device(device);
	tst_rmdir();
}

