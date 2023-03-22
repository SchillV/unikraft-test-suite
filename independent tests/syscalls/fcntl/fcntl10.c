#include "incl.h"
void setup();
void cleanup();
char *TCID = "fcntl10";
int TST_TOTAL = 2;
char fname[255];
int fd;
struct flock flocks;
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
		int type;
		for (type = 0; type < 2; type++) {
			tst_count = 0;
			flocks.l_type = type ? F_RDLCK : F_WRLCK;
			 * Call fcntl(2) with F_SETLKW flocks.l_type = F_UNLCK argument on fname
			 */
fcntl(fd, F_SETLKW, &flocks);
			if (TEST_RETURN == -1) {
				tst_resm(TFAIL,
					 "fcntl(%s, F_SETLKW, &flocks) flocks.l_type = %s Failed, errno=%d : %s",
					 fname, type ? "F_RDLCK" : "F_WRLCK",
					 TEST_ERRNO, strerror(TEST_ERRNO));
			} else {
				tst_resm(TPASS,
					 "fcntl(%s, F_SETLKW, &flocks) flocks.l_type = %s returned %ld",
					 fname, type ? "F_RDLCK" : "F_WRLCK",
					 TEST_RETURN);
			}
			flocks.l_type = F_UNLCK;
			 * Call fcntl(2) with F_SETLKW flocks.l_type = F_UNLCK argument on fname
			 */
fcntl(fd, F_SETLKW, &flocks);
			if (TEST_RETURN == -1) {
				tst_resm(TFAIL,
					 "fcntl(%s, F_SETLKW, &flocks) flocks.l_type = F_UNLCK Failed, errno=%d : %s",
					 fname, TEST_ERRNO,
					 strerror(TEST_ERRNO));
			} else {
				tst_resm(TPASS,
					 "fcntl(%s, F_SETLKW, &flocks) flocks.l_type = F_UNLCK returned %ld",
					 fname, TEST_RETURN);
			}
		}
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
	if ((fd = creat(fname, 0644)) == -1) {
		tst_brkm(TBROK, cleanup,
			 "creat(%s, 0644) Failed, errno=%d : %s", fname, errno,
			 strerror(errno));
	} else if (close(fd) == -1) {
		tst_brkm(TBROK, cleanup, "close(%s) Failed, errno=%d : %s",
			 fname, errno, strerror(errno));
	} else if ((fd = open(fname, O_RDWR, 0700)) == -1) {
		tst_brkm(TBROK, cleanup,
			 "open(%s, O_RDWR,0700) Failed, errno=%d : %s", fname,
			 errno, strerror(errno));
	}
	flocks.l_whence = 1;
	flocks.l_start = 0;
	flocks.l_len = 0;
	flocks.l_pid = getpid();
}
void cleanup(void)
{
	if (close(fd) == -1) {
		tst_resm(TWARN, "close(%s) Failed, errno=%d : %s", fname, errno,
			 strerror(errno));
	} else if (unlink(fname) == -1) {
		tst_resm(TWARN, "unlink(%s) Failed, errno=%d : %s", fname,
			 errno, strerror(errno));
	}
	tst_rmdir();
}

