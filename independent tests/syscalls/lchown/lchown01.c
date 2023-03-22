#include "incl.h"
#define FILE_MODE	(S_IFREG|S_IRUSR|S_IWUSR|S_IRGRP|S_IROTH)
#define TESTFILE	"testfile"
#define SFILE		"slink_file"
TCID_DEFINE(lchown01);
int TST_TOTAL = 5;
struct test_case_t {
	char *desc;
	uid_t user_id;
	gid_t group_id;
};

struct test_case_t test_cases[] = {
	{"Change Owner/Group ids", 700, 701},
	{"Change Owner id only", 702, -1},
	{"Change Owner/Group ids", 703, 701},
	{"Change Group id only", -1, 704},
	{"Change Group/Group ids", 703, 705},
	{"Change none", -1, -1},
	{NULL, 0, 0}
};

void setup(void);

void cleanup(void);
int main(int argc, char *argv[])
{
	struct stat stat_buf;
	int lc;
	int i;
	tst_parse_opts(argc, argv, NULL, NULL);
	setup();
	for (lc = 0; TEST_LOOPING(lc); lc++) {
		tst_count = 0;
		for (i = 0; test_cases[i].desc != NULL; i++) {
			uid_t user_id = test_cases[i].user_id;
			gid_t group_id = test_cases[i].group_id;
			char *test_desc = test_cases[i].desc;
			 * Call lchown(2) with different user id and
			 * group id (numeric values) to set it on
			 * symlink of testfile.
			 */
LCHOWN(cleanup, SFILE, user_id, group_id);
			if (TEST_RETURN == -1) {
				tst_resm(TFAIL,
					 "lchown() Fails to %s, errno %d",
					 test_desc, TEST_ERRNO);
				continue;
			}
			if (lstat(SFILE, &stat_buf) < 0) {
				tst_brkm(TFAIL, cleanup, "lstat(2) "
					 "%s failed, errno %d",
					 SFILE, TEST_ERRNO);
			}
			if ((int)user_id == -1) {
				if (i > 0)
					user_id =
					    test_cases[i - 1].user_id;
				else
					user_id = geteuid();
			}
			if ((int)group_id == -1) {
				if (i > 0)
					group_id =
					    test_cases[i - 1].group_id;
				else
					group_id = getegid();
			}
			 * Check for expected Ownership ids
			 * set on testfile.
			 */
			if ((stat_buf.st_uid != user_id) ||
			    (stat_buf.st_gid != group_id)) {
				tst_resm(TFAIL,
					 "%s: incorrect ownership set, "
					 "Expected %d %d", SFILE,
					 user_id, group_id);
			} else {
				tst_resm(TPASS, "lchown() succeeds to "
					 "%s of %s", test_desc, SFILE);
			}
		}
	}
	cleanup();
	tst_exit();
}

void setup(void)
{
	int fd;
	tst_sig(NOFORK, DEF_HANDLER, cleanup);
	tst_require_root();
	TEST_PAUSE;
	tst_tmpdir();
	if ((fd = open(TESTFILE, O_RDWR | O_CREAT, FILE_MODE)) == -1) {
		tst_brkm(TBROK, cleanup, "open failed");
	}
	close(cleanup, fd);
	symlink(cleanup, TESTFILE, SFILE);
}

void cleanup(void)
{
	tst_rmdir();
}

