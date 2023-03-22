#include "incl.h"
#define MODE_RWX		S_IFIFO | S_IRWXU | S_IRWXG | S_IRWXO
char Longpathname[PATH_MAX + 2];
	char *pathname;
	char *desc;
	int exp_errno;
	int (*setupfunc) ();
} Test_cases[] = {
	{"tnode_1", "Specified node already exists", EEXIST, setup1}, {
	NULL, "Invalid address", EFAULT, no_setup}, {
	"testdir_2/tnode_2", "Non-existent file", ENOENT, no_setup}, {
	"", "Pathname is empty", ENOENT, no_setup}, {
	Longpathname, "Pathname too long", ENAMETOOLONG, longpath_setup}, {
	"tnode/tnode_3", "Path contains regular file", ENOTDIR, setup3}, {
	NULL, NULL, 0, no_setup}
};
char *TCID = "mknod06";
int TST_TOTAL = ARRAY_SIZE(Test_cases);
int main(int ac, char **av)
{
	int lc;
	tst_parse_opts(ac, av, NULL, NULL);
	 * Invoke setup function to call individual test setup functions
	 * for the test which run as root/super-user.
	 */
	setup();
	for (lc = 0; TEST_LOOPING(lc); lc++) {
		tst_count = 0;
		for (ind = 0; Test_cases[ind].desc != NULL; ind++) {
			node_name = Test_cases[ind].pathname;
			test_desc = Test_cases[ind].desc;
			 * Call mknod(2) to test different test conditions.
int 			 * verify that it fails with -1 return value and
			 * sets appropriate errno.
			 */
mknod(node_name, MODE_RWX, 0);
			if (TEST_RETURN != -1) {
				tst_resm(TFAIL,
					 "mknod() returned %ld, expected "
					 "-1, errno:%d", TEST_RETURN,
					 Test_cases[ind].exp_errno);
				continue;
			}
			if (TEST_ERRNO == Test_cases[ind].exp_errno) {
				tst_resm(TPASS, "mknod() fails, %s, errno:%d",
					 test_desc, TEST_ERRNO);
			} else {
				tst_resm(TFAIL, "mknod() fails, %s, errno:%d, "
					 "expected errno:%d", test_desc,
					 TEST_ERRNO, Test_cases[ind].exp_errno);
			}
		}
	}
	 * Invoke cleanup() to delete the test directories created
	 * in the setup().
	 */
	cleanup();
	tst_exit();
}
void setup(void)
{
	int ind;
	tst_require_root();
	tst_sig(NOFORK, DEF_HANDLER, cleanup);
	TEST_PAUSE;
	tst_tmpdir();
	for (ind = 0; Test_cases[ind].desc != NULL; ind++) {
		if (!Test_cases[ind].pathname)
			Test_cases[ind].pathname = tst_get_bad_addr(cleanup);
		Test_cases[ind].setupfunc();
	}
}
int no_setup(void)
{
	return 0;
}
int longpath_setup(void)
{
	for (ind = 0; ind <= (PATH_MAX + 1); ind++) {
		Longpathname[ind] = 'a';
	}
	return 0;
}
int setup1(void)
{
	if (mknod("tnode_1", MODE_RWX, 0) < 0) {
		tst_brkm(TBROK, cleanup, "Fails to create node in setup1()");
	}
	return 0;
}
int setup3(void)
{
	if (mknod("tnode", MODE_RWX, 0) < 0) {
		tst_brkm(TBROK, cleanup, "Fails to create node in setup3()");
	}
	return 0;
}
void cleanup(void)
{
	tst_rmdir();
}

