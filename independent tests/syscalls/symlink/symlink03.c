#include "incl.h"
#define MODE_RWX        S_IRWXU | S_IRWXG | S_IRWXO
#define FILE_MODE       S_IRUSR | S_IRGRP | S_IROTH
#define DIR_TEMP        "testdir_1"
#define TESTFILE	"testfile"
#define TEST_FILE1      "testdir_1/tfile_1"
#define SYM_FILE1	"testdir_1/sfile_1"
#define TEST_FILE2      "tfile_2"
#define SYM_FILE2	"sfile_2"
#define TEST_FILE3      "tfile_3"
#define SYM_FILE3	"t_file/sfile_3"
char *TCID = "symlink03";
int TST_TOTAL = 1;
int no_setup();
char Longpathname[PATH_MAX + 2];
char High_address_node[64];
	char *file;
	char *link;
	char *desc;
	int exp_errno;
	int (*setupfunc) ();
} Test_cases[] = {
	{TEST_FILE1, SYM_FILE1, "No Search permissions to process",
		    EACCES, setup1}, {
	TEST_FILE2, SYM_FILE2, "Specified symlink already exists",
		    EEXIST, setup2}, {
	TESTFILE, NULL, "Invalid address", EFAULT, no_setup}, {
	TESTFILE, Longpathname, "Symlink path too long", ENAMETOOLONG,
		    longpath_setup}, {
	TESTFILE, "", "Symlink Pathname is empty", ENOENT, no_setup}, {
	TEST_FILE3, SYM_FILE3, "Symlink Path contains regular file",
		    ENOTDIR, setup3}, {
	NULL, NULL, NULL, 0, no_setup}
};
char nobody_uid[] = "nobody";
struct passwd *ltpuser;
void setup();
void cleanup();
int main(int ac, char **av)
{
	int lc;
	tst_parse_opts(ac, av, NULL, NULL);
	 * Invoke setup function to call individual test setup functions
	 * to simulate test conditions.
	 */
	setup();
	for (lc = 0; TEST_LOOPING(lc); lc++) {
		tst_count = 0;
		for (ind = 0; Test_cases[ind].desc != NULL; ind++) {
			test_file = Test_cases[ind].file;
			sym_file = Test_cases[ind].link;
			test_desc = Test_cases[ind].desc;
			 * Call symlink(2) to test different test conditions.
int 			 * verify that it fails with -1 return value and sets
			 * appropriate errno.
			 */
symlink(test_file, sym_file);
			if (TEST_RETURN == -1) {
				 * Perform functional verification if
				 * test executed without (-f) option.
				 */
				if (TEST_ERRNO == Test_cases[ind].exp_errno) {
					tst_resm(TPASS, "symlink() Fails, %s, "
						 "errno=%d", test_desc,
						 TEST_ERRNO);
				} else {
					tst_resm(TFAIL, "symlink() Fails, %s, "
						 "errno=%d, expected errno=%d",
						 test_desc, TEST_ERRNO,
						 Test_cases[ind].exp_errno);
				}
			} else {
				tst_resm(TFAIL, "symlink() returned %ld, "
					 "expected -1, errno:%d", TEST_RETURN,
					 Test_cases[ind].exp_errno);
			}
		}
	}
	cleanup();
	tst_exit();
}
void setup(void)
{
	int ind;
	tst_require_root();
	tst_sig(NOFORK, DEF_HANDLER, cleanup);
	 * TEST_PAUSE contains the code to fork the test with the -i option.
	 * You want to make sure you do this before you create your temporary
	 * directory.
	 */
	TEST_PAUSE;
	ltpuser = getpwnam(nobody_uid);
	if (setuid(ltpuser->pw_uid) == -1)
		tst_resm(TINFO | TERRNO, "setuid(%d) failed", ltpuser->pw_uid);
	tst_tmpdir();
	for (ind = 0; Test_cases[ind].desc != NULL; ind++) {
		if (!Test_cases[ind].link)
			Test_cases[ind].link = tst_get_bad_addr(cleanup);
		Test_cases[ind].setupfunc();
	}
}
int no_setup(void)
{
	return 0;
}
int setup1(void)
{
	mkdir(cleanup, DIR_TEMP, MODE_RWX);
	if ((fd = open(TEST_FILE1, O_RDWR | O_CREAT, 0666)) == -1) {
		tst_brkm(TBROK, cleanup,
			 "open(%s, O_RDWR|O_CREAT, 0666) failed, errno=%d : %s",
			 TEST_FILE1, errno, strerror(errno));
	}
	close(cleanup, fd);
	chmod(cleanup, DIR_TEMP, FILE_MODE);
	return 0;
}
int setup2(void)
{
	if ((fd = open(TEST_FILE2, O_RDWR | O_CREAT, 0666)) == -1) {
		tst_brkm(TBROK, cleanup,
			 "open(%s, O_RDWR|O_CREAT, 0666) failed, errno=%d : %s",
			 TEST_FILE1, errno, strerror(errno));
	}
	close(cleanup, fd);
	symlink(cleanup, TEST_FILE2, SYM_FILE2);
	return 0;
}
int longpath_setup(void)
{
	for (ind = 0; ind <= (PATH_MAX + 1); ind++) {
		Longpathname[ind] = 'a';
	}
	return 0;
}
int setup3(void)
{
	if ((fd = open("t_file", O_RDWR | O_CREAT, MODE_RWX)) == -1) {
		tst_brkm(TBROK, cleanup,
			 "open(2) on t_file failed, errno=%d : %s",
			 errno, strerror(errno));
	}
	close(cleanup, fd);
	return 0;
}
void cleanup(void)
{
	chmod(NULL, DIR_TEMP, MODE_RWX);
	tst_rmdir();
}

