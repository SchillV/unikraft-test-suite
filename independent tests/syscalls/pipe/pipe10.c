#include "incl.h"
char *TCID = "pipe10";
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
	char wrbuf[BUFSIZ], rebuf[BUFSIZ];
	int length, greater, forkstat;
	int retval = 0, status, e_code;
	tst_parse_opts(ac, av, NULL, NULL);
	setup();
	for (lc = 0; TEST_LOOPING(lc); lc++) {
		tst_count = 0;
pipe(fd);
		if (TEST_RETURN == -1) {
			retval = 1;
			tst_resm(TFAIL, "pipe creation failed");
			continue;
		}
		strcpy(wrbuf, "abcdefghijklmnopqrstuvwxyz");
		length = strlen(wrbuf) + 1;
		written = write(fd[1], wrbuf, length);
		if ((written < 0) || (written > length)) {
			tst_brkm(TBROK, cleanup, "write to pipe failed");
		}
		forkstat = FORK_OR_VFORK();
		if (forkstat == -1) {
			tst_brkm(TBROK, cleanup, "fork() failed");
		}
			red = do_read(fd[0], rebuf, written);
			if ((red < 0) || (red > written)) {
				tst_brkm(TBROK, cleanup, "read pipe failed");
			}
			greater = strcmp(rebuf, wrbuf);
			if (greater == 0) {
				tst_resm(TPASS, "functionality is correct");
			} else {
				retval = 1;
				tst_resm(TFAIL, "read & write strings do "
					 "not match");
			}
			exit(retval);
			wait(&status);
			e_code = status >> 8;
			if (e_code != 0) {
				tst_resm(TFAIL, "Failures reported above");
			}
		}
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

