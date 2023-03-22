#include "incl.h"
char *TCID = "pipe09";
int TST_TOTAL = 1;
void setup(void);
void cleanup(void);
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
	int i, red, wtstatus;
	char rebuf[BUFSIZ];
	tst_parse_opts(ac, av, NULL, NULL);
	setup();
	for (lc = 0; TEST_LOOPING(lc); lc++) {
		tst_count = 0;
pipe(pipefd);
		if (TEST_RETURN == -1) {
			tst_resm(TFAIL, "pipe() call failed");
			continue;
		}
		if ((fork_1 = FORK_OR_VFORK()) == -1) {
			tst_brkm(TBROK, cleanup, "fork() #1 failed");
		}
			if (close(pipefd[0]) != 0) {
				tst_resm(TWARN, "pipefd[0] close failed, "
					 "errno = %d", errno);
				exit(1);
			}
			for (i = 0; i < PIPEWRTCNT / 2; ++i) {
				if (write(pipefd[1], "A", 1) != 1) {
					tst_resm(TWARN, "write to pipe failed");
					exit(1);
				}
			}
			exit(0);
		}
		waitpid(cleanup, fork_1, &wtstatus, 0);
		if (WIFEXITED(wtstatus) && WEXITSTATUS(wtstatus) != 0) {
			tst_brkm(TBROK, cleanup, "child exited abnormally");
		}
		if ((fork_2 = FORK_OR_VFORK()) == -1) {
			tst_brkm(TBROK, cleanup, "fork() #2 failed");
		}
			if (close(pipefd[0]) != 0) {
				perror("pipefd[0] close failed");
				exit(1);
			}
			for (i = 0; i < PIPEWRTCNT / 2; ++i) {
				if (write(pipefd[1], "B", 1) != 1) {
					perror("write to pipe failed");
					exit(1);
				}
			}
			exit(0);
		}
		waitpid(cleanup, fork_2, &wtstatus, 0);
		if (WEXITSTATUS(wtstatus) != 0) {
			tst_brkm(TBROK, cleanup, "problem detected in child, "
				 "wait status %d, errno = %d", wtstatus, errno);
		}
		if (close(pipefd[1]) != 0) {
			tst_brkm(TBROK | TERRNO, cleanup,
				 "pipefd[1] close failed");
		}
		while ((red = do_read(pipefd[0], rebuf, 100)) > 0) {
			for (i = 0; i < red; i++) {
				if (rebuf[i] == 'A') {
					Acnt++;
					continue;
				}
				if (rebuf[i] == 'B') {
					Bcnt++;
					continue;
				}
				tst_resm(TFAIL, "got bogus '%c' character",
					 rebuf[i]);
				break;
			}
		}
		if (red == -1) {
			tst_brkm(TBROK | TERRNO, cleanup,
				 "reading pipefd pipe failed");
		}
		if (Bcnt == Acnt && Bcnt == (PIPEWRTCNT / 2)) {
			tst_resm(TPASS, "functionality appears to be correct");
		} else {
			tst_resm(TFAIL, "functionality is not correct - Acnt "
				 "= %d, Bcnt = %d", Acnt, Bcnt);
		}
		Acnt = Bcnt = 0;
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

