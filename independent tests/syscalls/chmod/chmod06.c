#include "incl.h"
#define MODE_RWX	(S_IRWXU|S_IRWXG|S_IRWXO)
#define FILE_MODE	(S_IRUSR|S_IWUSR|S_IRGRP|S_IROTH)
#define DIR_TEMP	"testdir_1"
#define TEST_FILE1	"tfile_1"
#define TEST_FILE2	"testdir_1/tfile_2"
#define TEST_FILE3	"t_file/tfile_3"
#define TEST_FILE4	"test_file4"
#define MNT_POINT	"mntpoint"
#define DIR_MODE	(S_IRUSR|S_IWUSR|S_IXUSR|S_IRGRP| \
			 S_IXGRP|S_IROTH|S_IXOTH)

char long_path[PATH_MAX + 2];

uid_t nobody_uid;

void set_root(void);

void set_nobody(void);

struct tcase {
	char *pathname;
	mode_t mode;
	int exp_errno;
	void (*setup)(void);
	void (*cleanup)(void);
} tc[] = {
	{TEST_FILE1, FILE_MODE, EPERM, set_nobody, set_root},
	{TEST_FILE2, FILE_MODE, EACCES, set_nobody, set_root},
	{(char *)-1, FILE_MODE, EFAULT, NULL, NULL},
	{NULL, FILE_MODE, EFAULT, NULL, NULL},
	{long_path, FILE_MODE, ENAMETOOLONG, NULL, NULL},
	{"", FILE_MODE, ENOENT, NULL, NULL},
	{TEST_FILE3, FILE_MODE, ENOTDIR, NULL, NULL},
	{MNT_POINT, FILE_MODE, EROFS, NULL, NULL},
	{TEST_FILE4, FILE_MODE, ELOOP, NULL, NULL},
};

char *bad_addr;
void run(unsigned int i)
{
	if (tc[i].setup)
		tc[i].setup();
chmod(tc[i].pathname, tc[i].mode);
	if (tc[i].cleanup)
		tc[i].cleanup();
	if (TST_RET != -1) {
		tst_res(TFAIL, "chmod succeeded unexpectedly");
		return;
	}
	if (TST_ERR == tc[i].exp_errno) {
		tst_res(TPASS | TTERRNO, "chmod failed as expected");
	} else {
		tst_res(TFAIL | TTERRNO, "chmod failed unexpectedly; "
		        "expected %d - %s", tc[i].exp_errno,
			tst_strerrno(tc[i].exp_errno));
	}
}
void set_root(void)
{
	seteuid(0);
}
void set_nobody(void)
{
	seteuid(nobody_uid);
}
void setup(void)
{
	struct passwd *nobody;
	unsigned int i;
	nobody = getpwnam("nobody");
	nobody_uid = nobody->pw_uid;
	bad_addr = mmap(0, 1, PROT_NONE, MAP_PRIVATE|MAP_ANONYMOUS, 0, 0);
	for (i = 0; i < ARRAY_SIZE(tc); i++) {
		if (!tc[i].pathname)
			tc[i].pathname = bad_addr;
	}
	touch(TEST_FILE1, 0666, NULL);
	mkdir(DIR_TEMP, MODE_RWX);
	touch(TEST_FILE2, 0666, NULL);
	chmod(DIR_TEMP, FILE_MODE);
	touch("t_file", MODE_RWX, NULL);
	memset(long_path, 'a', PATH_MAX+1);
	 * create two symbolic links who point to each other for
	 * test ELOOP.
	 */
	symlink("test_file4", "test_file5");
	symlink("test_file5", "test_file4");
}

void cleanup(void)
{
	chmod(DIR_TEMP, MODE_RWX);
}

void main(){
	setup();
	cleanup();
}
