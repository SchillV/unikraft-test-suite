#include "incl.h"
char *TCID = "pipe04";
int TST_TOTAL = 1;
void setup(void);
void cleanup(void);
void c1func(void);
void c2func(void);
void alarmfunc(int);
ssize_t do_read(int fd, void *buf, size_t count)
{
	ssize_t n;
	do {
		n = read(fd, buf, count);
	} while (n < 0 && errno == EINTR);
	return n;
}
int main(int ac, char **av)
{
	int lc;
	pid_t c1pid, c2pid;
	int wtstatus;
	int bytesread;
	int acnt = 0, bcnt = 0;
	char rbuf[BUFSIZ];
	tst_parse_opts(ac, av, NULL, NULL);
#ifdef UCLINUX
	maybe_run_child(&c1func, "ndd", 1, &fildes[0], &fildes[1]);
	maybe_run_child(&c2func, "ndd", 2, &fildes[0], &fildes[1]);
#endif
	setup();
	for (lc = 0; TEST_LOOPING(lc); lc++) {
		tst_count = 0;
		pipe(cleanup, fildes);
		if ((c1pid = FORK_OR_VFORK()) == -1)
			tst_brkm(TBROK, cleanup, "fork() failed - "
				 "errno %d", errno);
		if (c1pid == 0)
#ifdef UCLINUX
			if (self_exec(av[0], "ndd", 1, fildes[0], fildes[1]) <
			    0) {
				tst_brkm(TBROK, cleanup, "self_exec failed");
			}
#else
			c1func();
#endif
		if ((c2pid = FORK_OR_VFORK()) == -1)
			tst_brkm(TBROK, cleanup, "fork() failed - "
				 "errno %d", errno);
		if (c2pid == 0)
#ifdef UCLINUX
			if (self_exec(av[0], "ndd", 2, fildes[0], fildes[1]) <
			    0) {
				tst_brkm(TBROK, cleanup, "self_exec failed");
			}
#else
			c2func();
#endif
		if (close(fildes[1]) == -1)
			tst_resm(TWARN, "Could not close fildes[1] - errno %d",
				 errno);
		 * Read a bit from the children first
		 */
		while ((acnt < 100) && (bcnt < 100)) {
			bytesread = do_read(fildes[0], rbuf, sizeof(rbuf));
			if (bytesread < 0) {
				tst_resm(TFAIL, "Unable to read from pipe, "
					 "errno=%d", errno);
				break;
			}
			switch (rbuf[1]) {
			case 'A':
				acnt++;
				break;
			case 'b':
				bcnt++;
				break;
			default:
				tst_resm(TFAIL, "Got bogus '%c' "
					 "character", rbuf[1]);
				break;
			}
		}
		 * Try to kill the children
		 */
		if (kill(c1pid, SIGKILL) == -1)
			tst_resm(TFAIL, "failed to kill child 1, errno=%d",
				 errno);
		if (kill(c2pid, SIGKILL) == -1)
			tst_resm(TFAIL, "failed to kill child 1, errno=%d",
				 errno);
		 * Set action for the alarm
		 */
		if (signal(SIGALRM, alarmfunc) == SIG_ERR)
			tst_resm(TWARN | TERRNO, "call to signal failed");
		 * Set an alarm for 60 seconds just in case the child
		 * processes don't die
		 */
		alarm(60);
		if (waitpid(c1pid, &wtstatus, 0) != -1) {
			if (wtstatus != SIGKILL)
				tst_resm(TFAIL | TERRNO,
					 "unexpected wait status " "%d",
					 wtstatus);
			else
				tst_resm(TPASS, "Child 1 killed while "
					 "writing to a pipe");
		}
		if (waitpid(c2pid, &wtstatus, 0) != -1) {
			if (!WIFSIGNALED(wtstatus) ||
			    WTERMSIG(wtstatus) != SIGKILL)
				tst_resm(TFAIL | TERRNO,
					 "unexpected wait status " "%d",
					 wtstatus);
			else
				tst_resm(TPASS, "Child 2 killed while "
					 "writing to a pipe");
		}
		if (alarm(0) <= 0)
			tst_resm(TWARN, "call to alarm(0) failed");
	}
	cleanup();
	tst_exit();
}
void setup(void)
{
	tst_sig(FORK, DEF_HANDLER, cleanup);
	TEST_PAUSE;
}
void cleanup(void)
{
}
void c1func(void)
{
	if (close(fildes[0]) == -1)
		tst_resm(TWARN, "Could not close fildes[0] - errno %d", errno);
	while (1)
		if (write(fildes[1], "bbbbbbbbbbbbbbbbbbbbbbbbb", 25) == -1)
			tst_resm(TBROK | TERRNO, "[child 1] pipe write failed");
}
void c2func(void)
{
	if (close(fildes[0]) == -1)
		tst_resm(TWARN, "Could not close fildes[0] - errno %d", errno);
	while (1)
		if (write(fildes[1], "AAAAAAAAAAAAAAAAAAAAAAAAA", 25) == -1)
			tst_resm(TBROK | TERRNO, "[child 2] pipe write failed");
}
void alarmfunc(int sig LTP_ATTRIBUTE_UNUSED)
{
	tst_brkm(TFAIL, cleanup, "one or more children did't die in 60 second "
		 "time limit");
}

