#include "incl.h"
void setup();
void cleanup();
char *TCID = "fcntl27";
int TST_TOTAL = 1;
char fname[255];
int fd;
int main(int ac, char **av)
{
	int lc, expected_result = -1;
     * parse standard options
     ***************************************************************/
	tst_parse_opts(ac, av, NULL, NULL);
     * perform global setup for test
     ***************************************************************/
	setup();
	expected_result = -1;
     * check looping state if -c option given
     ***************************************************************/
	for (lc = 0; TEST_LOOPING(lc); lc++) {
		tst_count = 0;
#ifdef F_SETLEASE
		 * Call fcntl(2) with F_SETLEASE & F_RDLCK argument on fname
		 */
fcntl(fd, F_SETLEASE, F_RDLCK);
		if (TEST_RETURN == expected_result) {
			tst_resm(TPASS,
				 "fcntl(fd, F_SETLEASE, F_RDLCK) succeeded");
		} else {
			tst_resm(TFAIL, "fcntl(%s, F_SETLEASE, F_RDLCK)"
				 " failed with errno %d : %s", fname,
				 TEST_ERRNO, strerror(TEST_ERRNO));
		}
#else
		tst_resm(TINFO, "F_SETLEASE not defined, skipping test");
#endif
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
	if ((fd = open(fname, O_RDWR | O_CREAT, 0777)) == -1) {
		tst_brkm(TBROK, cleanup,
			 "open(%s, O_RDWR|O_CREAT,0777) Failed, errno=%d : %s",
			 fname, errno, strerror(errno));
	}
}
void cleanup(void)
{
	if (close(fd) == -1) {
		tst_resm(TWARN, "close(%s) Failed, errno=%d : %s", fname, errno,
			 strerror(errno));
	}
	tst_rmdir();
}

