#include "incl.h"
#ifndef PAGESIZE
#define PAGESIZE 4096
#endif
#define FAILED 1

void cleanup(void);

void setup(void);
char *TCID = "mprotect03";
int TST_TOTAL = 1;
int status;
char file1[BUFSIZ];
int main(int ac, char **av)
{
	int lc;
	char *addr;
	int fd, pid;
	char *buf = "abcdefghijklmnopqrstuvwxyz";
	tst_parse_opts(ac, av, NULL, NULL);
	setup();
	for (lc = 0; TEST_LOOPING(lc); lc++) {
		tst_count = 0;
		if ((fd = open(file1, O_RDWR | O_CREAT, 0777)) < 0)
			tst_brkm(TBROK, cleanup, "open failed");
		write(cleanup, 1, fd, buf, strlencleanup, 1, fd, buf, strlenbuf));
		 * mmap the PAGESIZE bytes as read only.
		 */
		addr = mmap(0, strlen(buf), PROT_READ | PROT_WRITE, MAP_SHARED,
			    fd, 0);
		if (addr == MAP_FAILED)
			tst_brkm(TBROK, cleanup, "mmap failed");
		 * Try to change the protection to WRITE.
		 */
mprotect(addr, strlen(buf), PROT_READ);
		if (TEST_RETURN != -1) {
			if ((pid = FORK_OR_VFORK()) == -1) {
				tst_brkm(TBROK, cleanup, "fork failed");
			}
			if (pid == 0) {
				memcpy(addr, buf, strlen(buf));
				tst_resm(TINFO, "memcpy() did "
					 "not generate SIGSEGV");
				exit(1);
			}
			waitpid(pid, &status, 0);
			if (WEXITSTATUS(status) != 0) {
				tst_resm(TFAIL, "child returned "
					 "unexpected status");
			} else {
				tst_resm(TPASS, "SIGSEGV generated "
					 "as expected");
			}
		} else {
			tst_resm(TFAIL, "mprotect failed "
				 "unexpectedly, errno: %d", errno);
		}
		munmap(cleanup, addr, strlencleanup, addr, strlenbuf));
		close(cleanup, fd);
		unlink(cleanup, file1);
	}
	cleanup();
	tst_exit();
}

void sighandler(int sig)
{
	if (sig == SIGSEGV) {
		tst_resm(TINFO, "received signal: SIGSEGV");
		tst_exit();
	} else
		tst_brkm(TBROK, 0, "Unexpected signal %d received.", sig);
}

void setup(void)
{
	tst_sig(FORK, sighandler, NULL);
	TEST_PAUSE;
	tst_tmpdir();
	sprintf(file1, "mprotect03.tmp.%d", getpid());
}

void cleanup(void)
{
	tst_rmdir();
}

