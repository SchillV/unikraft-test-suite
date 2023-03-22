#include "incl.h"
#define TEST_USER       "nobody"
#define MODE_RWX	S_IRWXU | S_IRWXG | S_IRWXO
#define FILE_MODE	S_IRUSR | S_IWUSR | S_IRGRP | S_IROTH
#define DIR_TEMP	"testdir_1"
#define TEST_FILE1	"tfile_1"
#define SFILE1		"sfile_1"
#define TEST_FILE2	"testdir_1/tfile_2"
#define SFILE2		"testdir_1/sfile_2"
#define TFILE3          "t_file"
#define SFILE3		"t_file/sfile"
TCID_DEFINE(lchown02);
int TST_TOTAL = 7;

void setup(void);

void cleanup(void);

void setup_eperm(int pos);

void setup_eacces(int pos);

void setup_enotdir(int pos);

void setup_longpath(int pos);

void setup_efault(int pos);

char path[PATH_MAX + 2];
struct test_case_t {
	char *pathname;
	char *desc;
	int exp_errno;
	void (*setup) (int pos);
};

struct test_case_t test_cases[] = {
	{SFILE1, "Process is not owner/root", EPERM, setup_eperm},
	{SFILE2, "Search permission denied", EACCES, setup_eacces},
	{NULL, "Unaccessible address space", EFAULT, setup_efault},
	{path, "Pathname too long", ENAMETOOLONG, setup_longpath},
	{SFILE3, "Path contains regular file", ENOTDIR, setup_enotdir},
	{"", "Pathname is empty", ENOENT, NULL},
	{NULL, NULL, 0, NULL}
};

struct passwd *ltpuser;
int main(int argc, char *argv[])
{
	int lc;
	uid_t user_id;
	gid_t group_id;
	int i;
	tst_parse_opts(argc, argv, NULL, NULL);
	setup();
	user_id = geteuid();
	UID16_CHECK(user_id, lchown, cleanup);
	group_id = getegid();
	GID16_CHECK(group_id, lchown, cleanup);
	for (lc = 0; TEST_LOOPING(lc); lc++) {
		tst_count = 0;
		for (i = 0; test_cases[i].desc != NULL; i++) {
			char *file_name = test_cases[i].pathname;
			char *test_desc = test_cases[i].desc;
			 * Call lchown(2) to test different test conditions.
int 			 * verify that it fails with -1 return value and
			 * sets appropriate errno.
			 */
LCHOWN(cleanup, file_name, user_id, group_id);
			if (TEST_RETURN == -1) {
				if (TEST_ERRNO == test_cases[i].exp_errno) {
					tst_resm(TPASS,
						 "lchown(2) fails, %s, errno:%d",
						 test_desc, TEST_ERRNO);
				} else {
					tst_resm(TFAIL, "lchown(2) fails, %s, "
						 "errno:%d, expected errno:%d",
						 test_desc, TEST_ERRNO,
						 test_cases[i].exp_errno);
				}
			} else {
				tst_resm(TFAIL, "lchown(2) returned %ld, "
					 "expected -1, errno:%d", TEST_RETURN,
					 test_cases[i].exp_errno);
			}
		}
	}
	cleanup();
	tst_exit();
}

void setup(void)
{
	int i;
	tst_sig(FORK, DEF_HANDLER, cleanup);
	tst_require_root();
	TEST_PAUSE;
	ltpuser = getpwnam(TEST_USER);
	if (ltpuser == NULL)
		tst_brkm(TBROK, cleanup, "getpwnam failed");
	if (setgid(ltpuser->pw_uid) == -1)
		tst_resm(TBROK | TERRNO, "setgid failed");
	tst_tmpdir();
	for (i = 0; test_cases[i].desc != NULL; i++)
		if (test_cases[i].setup != NULL)
			test_cases[i].setup(i);
}

void setup_eperm(int pos LTP_ATTRIBUTE_UNUSED)
{
	int fd;
	if ((fd = open(TEST_FILE1, O_RDWR | O_CREAT, 0666)) == -1)
		tst_brkm(TBROK | TERRNO, cleanup, "open failed");
	close(cleanup, fd);
	if (seteuid(0) == -1)
		tst_resm(TBROK | TERRNO, "setuid(0) failed");
	symlink(cleanup, TEST_FILE1, SFILE1);
	if (seteuid(ltpuser->pw_uid) == -1)
		tst_resm(TBROK | TERRNO, "seteuid(%d) failed", ltpuser->pw_uid);
}

void setup_eacces(int pos LTP_ATTRIBUTE_UNUSED)
{
	int fd;
	mkdir(cleanup, DIR_TEMP, MODE_RWX);
	if ((fd = open(TEST_FILE2, O_RDWR | O_CREAT, 0666)) == -1)
		tst_brkm(TBROK | TERRNO, cleanup, "open failed");
	close(cleanup, fd);
	symlink(cleanup, TEST_FILE2, SFILE2);
	chmod(cleanup, DIR_TEMP, FILE_MODE);
}

void setup_efault(int pos)
{
	test_cases[pos].pathname = tst_get_bad_addr(cleanup);
}

void setup_enotdir(int pos LTP_ATTRIBUTE_UNUSED)
{
	int fd;
	if ((fd = open(TFILE3, O_RDWR | O_CREAT, MODE_RWX)) == -1) {
		tst_brkm(TBROK | TERRNO, cleanup, "open(2) %s failed", TFILE3);
	}
	close(cleanup, fd);
}

void setup_longpath(int pos)
{
	memset(test_cases[pos].pathname, 'a', PATH_MAX + 1);
	test_cases[pos].pathname[PATH_MAX + 1] = '\0';
}

void cleanup(void)
{
	if (seteuid(0) == -1) {
		tst_resm(TINFO | TERRNO,
			 "seteuid(2) failed to set the effective uid to 0");
	}
	tst_rmdir();
}

