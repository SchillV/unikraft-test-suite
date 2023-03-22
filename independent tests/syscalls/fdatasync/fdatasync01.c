#include "incl.h"

int fd;

char filename[30];

void setup(void);

void cleanup(void);
char *TCID = "fdatasync01";
int TST_TOTAL = 1;
int main(int argc, char **argv)
{
	int lc;
	tst_parse_opts(argc, argv, NULL, NULL);
	setup();
	for (lc = 0; TEST_LOOPING(lc); lc++) {
		tst_count = 0;
fdatasync(fd);
		if (TEST_RETURN == -1) {
			tst_resm(TFAIL, "fdatasync() failed, errno=%d : %s",
				 TEST_ERRNO, strerror(TEST_ERRNO));
		} else {
			tst_resm(TPASS, "fdatasync() successful");
		}
	}
	cleanup();
	tst_exit();
}
void setup(void)
{
	tst_sig(NOFORK, DEF_HANDLER, cleanup);
	 * TEST_PAUSE contains the code to fork the test with the -c option.
	 */
	TEST_PAUSE;
	tst_tmpdir();
	if (sprintf(filename, "fdatasync_%d", getpid()) <= 0) {
		tst_brkm(TBROK, cleanup, "Failed to initialize filename");
	}
		tst_brkm(TBROK, cleanup, "open() failed");
	}
	if ((write(fd, filename, strlen(filename) + 1)) == -1) {
		tst_brkm(TBROK, cleanup, "write() failed");
	}
}
void cleanup(void)
{
	 * print timing stats if that option was specified.
	 * print errno log if that option was specified.
	 */
	close(fd);
	tst_rmdir();
}

