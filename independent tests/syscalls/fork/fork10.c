#include "incl.h"
char *TCID = "fork10";
int TST_TOTAL = 1;

void setup(void);

void cleanup(void);

char pidbuf[10];

char fnamebuf[40];
int main(int ac, char **av)
{
	int status, pid, fildes;
	char parchar[2];
	char chilchar[2];
	int lc;
	fildes = -1;
	tst_parse_opts(ac, av, NULL, NULL);
	setup();
	for (lc = 0; TEST_LOOPING(lc); lc++) {
		tst_count = 0;
		fildes = creat(cleanup, fnamebuf, 0600);
		write(fildes, "ABCDEFGHIJKLMNOPQRSTUVWXYZ\n", 27);
		close(fildes);
		fildes = open(cleanup, fnamebuf, 0);
		pid = fork();
		if (pid == -1)
			tst_brkm(TBROK, cleanup, "fork() #1 failed");
			tst_resm(TINFO, "fork child A");
			if (lseek(fildes, 10L, 0) == -1L) {
				tst_resm(TFAIL, "bad lseek by child");
				exit(1);
			}
			exit(0);
			wait(&status);
			pid = fork();
			if (pid == -1)
				tst_brkm(TBROK, cleanup, "fork() #2 failed");
				if (read(fildes, chilchar, 1) <= 0) {
					tst_resm(TFAIL, "Child can't read "
						 "file");
					exit(1);
				} else {
					if (chilchar[0] != 'K') {
						chilchar[1] = '\n';
						exit(1);
					} else {
						exit(0);
					}
				}
				(void)wait(&status);
				if (status >> 8 != 0) {
					tst_resm(TFAIL, "Bad return from "
						 "second child");
					continue;
				}
				if (read(fildes, parchar, 1) <= 0) {
					tst_resm(TFAIL, "Parent cannot read "
						 "file");
					continue;
				} else {
					write(fildes, parchar, 1);
					if (parchar[0] != 'L') {
						parchar[1] = '\n';
						tst_resm(TFAIL, "Test failed");
						continue;
					}
				}
			}
		}
		tst_resm(TPASS, "test 1 PASSED");
	}
	close(fildes);
	cleanup();
	tst_exit();
}

void setup(void)
{
	tst_sig(FORK, DEF_HANDLER, cleanup);
	umask(0);
	TEST_PAUSE;
	tst_tmpdir();
	strcpy(fnamebuf, "fork10.");
	sprintf(pidbuf, "%d", getpid());
	strcat(fnamebuf, pidbuf);
}

void cleanup(void)
{
	tst_rmdir();
}

