#include "incl.h"
#define MODE 0666
#define MODE_RWX	(S_IRWXU|S_IRWXG|S_IRWXO)
#define FILE_MODE	(S_IRUSR|S_IWUSR|S_IRGRP|S_IROTH)
#define DIR_MODE	(S_IRUSR|S_IWUSR|S_IXUSR|S_IRGRP|S_IXGRP|S_IROTH|S_IXOTH)
#define MNT_POINT	"mntpoint"
#define DIR_TEMP	"testdir_1"
#define TEST_FILE1	"tfile_1"
#define TEST_FILE2	"testdir_1/tfile_2"
#define TEST_FILE3	"t_file/tfile_3"
#define TEST_FILE4	"test_eloop1"
#define TEST_FILE5	"mntpoint"

char long_path[PATH_MAX + 2] = {[0 ... PATH_MAX + 1] = 'a'};

struct test_case_t {
	char *pathname;
	int exp_errno;
	char *desc;
} tc[] = {
	{TEST_FILE1, EPERM, "without permissions"},
	{TEST_FILE2, EACCES, "without full permissions of the path prefix"},
	{(char *)-1, EFAULT, "with unaccessible pathname points"},
	{long_path, ENAMETOOLONG, "when pathname is too long"},
	{"", ENOENT, "when file does not exist"},
	{TEST_FILE3, ENOTDIR, "when the path prefix is not a directory"},
	{TEST_FILE4, ELOOP, "with too many symbolic links"},
	{TEST_FILE5, EROFS, "when the named file resides on a read-only filesystem"}
};

void run(unsigned int i)
{
	uid_t uid;
	gid_t gid;
	UID16_CHECK((uid = geteuid()), "chown");
	GID16_CHECK((gid = getegid()), "chown");
	TST_EXP_FAIL(CHOWN(tc[i].pathname, uid, gid), tc[i].exp_errno,
		     "chown() %s", tc[i].desc);
}

void setup(void)
{
	struct passwd *ltpuser;
	tc[2].pathname = tst_get_bad_addr(NULL);
	symlink("test_eloop1", "test_eloop2");
	symlink("test_eloop2", "test_eloop1");
	seteuid(0);
	touch("t_file", MODE_RWX, NULL);
	touch(TEST_FILE1, MODE, NULL);
	mkdir(DIR_TEMP, S_IRWXU);
	touch(TEST_FILE2, MODE, NULL);
	ltpuser = getpwnam("nobody");
	seteuid(ltpuser->pw_uid);
}

void main(){
	setup();
	cleanup();
}
