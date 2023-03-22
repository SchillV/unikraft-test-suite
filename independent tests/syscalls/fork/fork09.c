#include "incl.h"
char *TCID = "fork09";
int TST_TOTAL = 1;

void setup(void);

void cleanup(void);

char filname[40], childfile[40];

int first;


int mypid, nfiles;
#define OPEN_MAX (sysconf(_SC_OPEN_MAX))
int main(int ac, char **av)
{
	int pid, status, nf;
	int lc;
	tst_parse_opts(ac, av, NULL, NULL);
	setup();
	fildeses = malloc((OPEN_MAX + 10) * sizeof(FILE *));
	if (fildeses == NULL)
		tst_brkm(TBROK, cleanup, "malloc failed");
	for (lc = 0; TEST_LOOPING(lc); lc++) {
		tst_count = 0;
		mypid = getpid();
		tst_resm(TINFO, "OPEN_MAX is %ld", OPEN_MAX);
		sprintf(filname, "fork09.%d", mypid);
		first = creat(cleanup, filname, 0660);
		close(first);
		tst_resm(TINFO, "first file descriptor is %d ", first);
		unlink(cleanup, filname);
		 * now open all the files for the test
		 */
		for (nfiles = first; nfiles < OPEN_MAX; nfiles++) {
			sprintf(filname, "file%d.%d", nfiles, mypid);
			fildeses[nfiles] = fopen(filname, "a");
			if (fildeses[nfiles] == NULL) {
				if (errno == EMFILE)
					break;
				tst_brkm(TBROK, cleanup, "Parent: cannot open "
					 "file %d %s errno = %d", nfiles,
					 filname, errno);
			}
#ifdef DEBUG
			tst_resm(TINFO, "filname: %s", filname);
#endif
		}
		tst_resm(TINFO, "Parent reporting %d files open", nfiles - 1);
		pid = fork();
		if (pid == -1)
			tst_brkm(TBROK, cleanup, "Fork failed");
			nfiles--;
			if (fclose(fildeses[nfiles]) == -1) {
				tst_resm(TINFO, "Child could not close file "
					 "#%d, errno = %d", nfiles, errno);
				exit(1);
			} else {
				sprintf(childfile, "cfile.%d", getpid());
				fildeses[nfiles] = fopen(childfile, "a");
				if (fildeses[nfiles] == NULL) {
					tst_resm(TINFO, "Child could not open "
						 "file %s, errno = %d",
						 childfile, errno);
					exit(1);
				} else {
					tst_resm(TINFO, "Child opened new "
						 "file #%d", nfiles);
					unlink(childfile);
					exit(0);
				}
			}
			wait(&status);
			if (status >> 8 != 0)
				tst_resm(TFAIL, "test 1 FAILED");
			else
				tst_resm(TPASS, "test 1 PASSED");
		}
		for (nf = first; nf < nfiles; nf++) {
			fclose(fildeses[nf]);
			sprintf(filname, "file%d.%d", nf, mypid);
			unlink(filname);
		}
	}
	cleanup();
	tst_exit();
}

void setup(void)
{
	tst_sig(FORK, DEF_HANDLER, cleanup);
	umask(0);
	TEST_PAUSE;
	tst_tmpdir();
}

void cleanup(void)
{
	tst_rmdir();
}

