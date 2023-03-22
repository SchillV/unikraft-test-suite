#include "incl.h"
#define  TESTFILE	"testfile"
#define  SYMFILE	"slink_file"
char *TCID = "symlink05";
int TST_TOTAL = 1;
void setup();
void cleanup();
int main(int ac, char **av)
{
	int lc;
	tst_parse_opts(ac, av, NULL, NULL);
	setup();
	for (lc = 0; TEST_LOOPING(lc); lc++) {
		tst_count = 0;
		 * Call symlink(2) to create a symlink of
		 * an non-existing testfile.
		 */
symlink(TESTFILE, SYMFILE);
		if (TEST_RETURN == -1) {
			tst_resm(TFAIL,
				 "symlink(%s, %s) Failed, errno=%d : %s",
				 TESTFILE, SYMFILE, TEST_ERRNO,
				 strerror(TEST_ERRNO));
		} else {
			 * Get the symlink file status information
			 * using lstat(2).
			 */
			if (lstat(SYMFILE, &stat_buf) < 0) {
				tst_brkm(TFAIL, cleanup, "lstat(2) of "
					 "%s failed, error:%d",
					 SYMFILE, errno);
			}
			if (!S_ISLNK(stat_buf.st_mode)) {
				tst_resm(TFAIL,
					 "symlink of %s doesn't exist",
					 TESTFILE);
			} else {
				tst_resm(TPASS, "symlink(%s, %s) "
					 "functionality successful",
					 TESTFILE, SYMFILE);
			}
		}
		unlink(cleanup, SYMFILE);
	}
	cleanup();
	tst_exit();
}
void setup(void)
{
	tst_sig(NOFORK, DEF_HANDLER, cleanup);
	 * TEST_PAUSE contains the code to fork the test with the -i option.
	 * You want to make sure you do this before you create your temporary
	 * directory.
	 */
	TEST_PAUSE;
	tst_tmpdir();
}
void cleanup(void)
{
	tst_rmdir();
}

