#include "incl.h"
void setup();
void cleanup();
char *TCID = "fcntl23";
int TST_TOTAL = 1;
char fname[255];
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
	if (tst_fs_type(cleanup, ".") == TST_NFS_MAGIC) {
		tst_brkm(TCONF, cleanup,
			 "Cannot do fcntl on a file on NFS filesystem");
	}
     * check looping state if -c option given
     ***************************************************************/
	for (lc = 0; TEST_LOOPING(lc); lc++) {
		tst_count = 0;
#ifdef F_SETLEASE
		 * Call fcntl(2) with F_SETLEASE & F_RDLCK argument on fname
		 */
fcntl(fd, F_SETLEASE, F_RDLCK);
		if (TEST_RETURN == -1) {
			tst_resm(TFAIL,
				 "fcntl(%s, F_SETLEASE, F_RDLCK) Failed, errno=%d : %s",
				 fname, TEST_ERRNO, strerror(TEST_ERRNO));
		} else {
fcntl(fd, F_GETLEASE);
			if (TEST_RETURN != F_RDLCK)
				tst_resm(TFAIL,
					 "fcntl(%s, F_GETLEASE) did not return F_RDLCK, returned %ld",
					 fname, TEST_RETURN);
			else {
fcntl(fd, F_SETLEASE, F_UNLCK);
				if (TEST_RETURN != 0)
					tst_resm(TFAIL,
						 "fcntl(%s, F_SETLEASE, F_UNLCK) did not return 0, returned %ld",
						 fname, TEST_RETURN);
				else
					tst_resm(TPASS,
						 "fcntl(%s, F_SETLEASE, F_RDLCK)",
						 fname);
			}
			if (close(TEST_RETURN) == -1) {
				tst_resm(TWARN,
					 "close(%s) Failed, errno=%d : %s",
					 fname, errno, strerror(errno));
			}
		}
#else
		tst_resm(TINFO, "F_SETLEASE not defined, skipping test");
#endif
	}
	cleanup();
	tst_exit();
}
void setup(void)
{
	tst_sig(NOFORK, DEF_HANDLER, cleanup);
	TEST_PAUSE;
	tst_tmpdir();
	sprintf(fname, "tfile_%d", getpid());
	if ((fd = open(fname, O_RDONLY | O_CREAT, 0777)) == -1) {
		tst_brkm(TBROK, cleanup,
			 "open(%s, O_RDONLY|O_CREAT,0777) Failed, errno=%d : %s",
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

