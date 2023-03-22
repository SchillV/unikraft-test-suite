#include "incl.h"
  WHAT:  Does dup return -1 on the 21st file?
  HOW:   Create up to _NFILE (20) files and check for -1 return on the
         next attempt
         Should check NOFILE as well as _NFILE.  19-Jun-84 Dale.
*/
char *TCID = "dup06";
int TST_TOTAL = 1;

int cnt_free_fds(int maxfd)
{
	int freefds = 0;
	for (maxfd--; maxfd >= 0; maxfd--)
		if (fcntl(maxfd, F_GETFD) == -1 && errno == EBADF)
			freefds++;
	return (freefds);
}

void setup(void);

void cleanup(void);
int main(int ac, char **av)
{
	int *fildes, i;
	int min;
	int freefds;
	int lc;
	const char *pfilname = "dup06";
	tst_parse_opts(ac, av, NULL, NULL);
	setup();
	min = getdtablesize();
	freefds = cnt_free_fds(min);
	fildes = malloc((min + 5) * sizeof(int));
	for (i = 0; i < min + 5; i++)
		fildes[i] = 0;
	for (lc = 0; TEST_LOOPING(lc); lc++) {
		unlink(pfilname);
		if ((fildes[0] = creat(pfilname, 0666)) == -1) {
			tst_resm(TFAIL, "Cannot open first file");
		} else {
			for (i = 1; i < min + 5; i++) {
				if ((fildes[i] = dup(fildes[i - 1])) == -1)
					break;
			}
			if (i < freefds) {
				tst_resm(TFAIL, "Not enough files duped");
			} else if (i > freefds) {
				tst_resm(TFAIL, "Too many files duped");
			} else {
				tst_resm(TPASS, "Test passed.");
			}
		}
		unlink(pfilname);
		for (i = 0; i < min + 5; i++) {
			if (fildes[i] != 0 && fildes[i] != -1)
				close(fildes[i]);
			fildes[i] = 0;
		}
	}
	cleanup();
	tst_exit();
}

void setup(void)
{
	tst_tmpdir();
}

void cleanup(void)
{
	tst_rmdir();
}

