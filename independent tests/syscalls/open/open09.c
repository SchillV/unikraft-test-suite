#include "incl.h"
char *TCID = "open09";
int TST_TOTAL = 2;
#define PASSED 1
#define FAILED 0

char tempfile[40] = "";

void setup(void);

void cleanup(void);
int main(int ac, char *av[])
{
	int fildes;
	int ret = 0;
	char pbuf[BUFSIZ];
	int lc;
	tst_parse_opts(ac, av, NULL, NULL);
	setup();
	for (lc = 0; TEST_LOOPING(lc); lc++) {
		fildes = open(tempfile, O_WRONLY);
		if (fildes == -1)
			tst_brkm(TFAIL, cleanup, "\t\t\topen failed");
		ret = read(fildes, pbuf, 1);
		if (ret != -1)
			tst_resm(TFAIL, "read should not succeed");
		else
			tst_resm(TPASS, "Test passed in O_WRONLY.");
		close(fildes);
		fildes = open(tempfile, O_RDONLY);
		if (fildes == -1) {
			tst_resm(TFAIL, "\t\t\topen failed");
		} else {
			ret = write(fildes, pbuf, 1);
			if (ret != -1)
				tst_resm(TFAIL, "write should not succeed");
			else
				tst_resm(TPASS, "Test passed in O_RDONLY.");
		}
		close(fildes);
	}
	cleanup();
	tst_exit();
}

void setup(void)
{
	int fildes;
	tst_sig(NOFORK, DEF_HANDLER, cleanup);
	TEST_PAUSE;
	tst_tmpdir();
	sprintf(tempfile, "open09.%d", getpid());
	fildes = creat(tempfile, 0600);
	if (fildes == -1) {
		tst_brkm(TBROK, cleanup, "\t\t\tcan't create '%s'",
			 tempfile);
	} else {
		close(fildes);
	}
}

void cleanup(void)
{
	unlink(tempfile);
	tst_rmdir();
}

