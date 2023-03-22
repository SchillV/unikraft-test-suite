#include "incl.h"
char *TCID = "fcntl06";
int TST_TOTAL = 1;
void setup();
void cleanup();
#define STRINGSIZE	27
#define	STRING		"abcdefghijklmnopqrstuvwxyz\n"
int fd;
void unlock_file();
int do_lock(int, short, short, int, int);
int main(int ac, char **av)
{
	int fail = 0;
	tst_parse_opts(ac, av, NULL, NULL);
	fail = 0;
#ifdef LINUX_FILE_REGION_LOCK
	if (fcntl(fd, F_RGETLK, &tl) == -1) {
		if (errno == EINVAL)
			tst_brkm(TCONF, cleanup,
				 "fcntl remote locking feature not implemented in "
				 "the kernel");
		else {
			 * FIXME (garrcoop): having it always pass on
			 * non-EINVAL is a bad test.
			 */
			tst_resm(TPASS, "fcntl on file failed");
		}
	}
	 * Add a write lock to the middle of the file and unlock a section
	 * just before the lock
	 */
	if (do_lock(F_RSETLK, F_WRLCK, 0, 10, 5) == -1)
		tst_resm(TFAIL, "F_RSETLK WRLCK failed");
	if (do_lock(F_RSETLK, F_UNLCK, 0, 5, 5) == -1)
		tst_resm(TFAIL | TERRNO, "F_RSETLK UNLOCK failed");
	unlock_file();
#else
	tst_resm(TCONF, "system doesn't have LINUX_LOCK_FILE_REGION support");
#endif
	cleanup();
	tst_exit();
}
void setup(void)
{
	char *buf = STRING;
	char template[PATH_MAX];
	tst_sig(FORK, DEF_HANDLER, cleanup);
	umask(0);
	TEST_PAUSE;
	tst_tmpdir();
	snprintf(template, PATH_MAX, "fcntl06XXXXXX");
	if ((fd = mkstemp(template)) == -1)
		tst_resm(TBROK | TERRNO, "mkstemp failed");
	if (write(fd, buf, STRINGSIZE) == -1)
		tst_resm(TBROK | TERRNO, "write failed");
}
int do_lock(int cmd, short type, short whence, int start, int len)
{
	struct flock fl;
	fl.l_type = type;
	fl.l_whence = whence;
	fl.l_start = start;
	fl.l_len = len;
	return (fcntl(fd, cmd, &fl));
}
void unlock_file(void)
{
	if (do_lock(F_RSETLK, (short)F_UNLCK, (short)0, 0, 0) == -1) {
		tst_resm(TPASS | TERRNO, "fcntl on file failed");
	}
}
void cleanup(void)
{
	if (close(fd) == -1)
		tst_resm(TWARN | TERRNO, "close failed");
	tst_rmdir();
}

