#include "incl.h"
void setup();
void cleanup();
char *TCID = "symlink02";
int TST_TOTAL = 1;
char fname[255], symlnk[255];
int fd;
int main(int ac, char **av)
{
	int lc;
     * parse standard options
     ***************************************************************/
	tst_parse_opts(ac, av, NULL, NULL);
     * perform global setup for test
     ***************************************************************/
	setup();
     * check looping state if -c option given
     ***************************************************************/
	for (lc = 0; TEST_LOOPING(lc); lc++) {
		tst_count = 0;
		 * Call symlink(2)
		 */
symlink(fname, symlnk);
		if (TEST_RETURN == -1) {
			tst_resm(TFAIL, "symlink(%s, %s) Failed, errno=%d : %s",
				 fname, symlnk, TEST_ERRNO,
				 strerror(TEST_ERRNO));
		} else {
			unlink(cleanup, symlnk);
		}
	}
     * cleanup and exit
     ***************************************************************/
	cleanup();
	tst_exit();
}
void setup(void)
{
	tst_sig(NOFORK, DEF_HANDLER, cleanup);
	TEST_PAUSE;
	tst_tmpdir();
	sprintf(fname, "tfile_%d", getpid());
	if ((fd = open(fname, O_RDWR | O_CREAT, 0700)) == -1) {
		tst_brkm(TBROK, cleanup,
			 "open(%s, O_RDWR|O_CREAT,0700) Failed, errno=%d : %s",
			 fname, errno, strerror(errno));
	}
	if (close(fd) == -1) {
		tst_resm(TWARN, "close(%s) Failed, errno=%d : %s",
			 fname, errno, strerror(errno));
	}
	sprintf(symlnk, "st_%d", getpid());
}
void cleanup(void)
{
	tst_rmdir();
}

