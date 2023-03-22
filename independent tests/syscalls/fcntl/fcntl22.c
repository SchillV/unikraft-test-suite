#include "incl.h"
int child_pid;
int file;
struct flock fl;
char *TCID = "fcntl22";
int TST_TOTAL = 1;
void setup(void);
void cleanup(void);
int main(int ac, char **av)
{
	int lc;
	tst_parse_opts(ac, av, NULL, NULL);
	setup();
	for (lc = 0; TEST_LOOPING(lc); lc++) {
		tst_count = 0;
		child_pid = FORK_OR_VFORK();
		switch (child_pid) {
		case 0:
fcntl(file, F_SETLK, &fl);
			if (TEST_RETURN != -1) {
				tst_resm(TFAIL, "fcntl() returned %ld, "
					 "expected -1, errno=%d", TEST_RETURN,
					 EAGAIN);
			} else {
				if (TEST_ERRNO == EAGAIN) {
					tst_resm(TPASS,
						 "fcntl() fails with expected "
						 "error EAGAIN errno:%d",
						 TEST_ERRNO);
				} else {
					tst_resm(TFAIL, "fcntl() fails, EAGAIN, "
						 "errno=%d, expected errno=%d",
						 TEST_ERRNO, EAGAIN);
				}
			}
			tst_exit();
		break;
		case -1:
			tst_brkm(TBROK|TERRNO, cleanup, "Fork failed");
		break;
		default:
			tst_record_childstatus(cleanup, child_pid);
		}
	}
	cleanup();
	tst_exit();
}
void setup(void)
{
	tst_sig(FORK, DEF_HANDLER, cleanup);
	TEST_PAUSE;
	tst_tmpdir();
	if ((file = creat("regfile", 0777)) == -1) {
		tst_brkm(TBROK, cleanup,
			 "creat(regfile, 0777) failed, errno:%d %s", errno,
			 strerror(errno));
	}
	fl.l_type = F_WRLCK;
	fl.l_whence = 0;
	fl.l_start = 0;
	fl.l_len = 0;
	if (fcntl(file, F_SETLK, &fl) < 0)
		tst_brkm(TBROK | TERRNO, cleanup, "fcntl() failed");
}
void cleanup(void)
{
	close(file);
	tst_rmdir();
}

