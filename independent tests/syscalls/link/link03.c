#include "incl.h"
  * Tests that link(2) succeds with creating n links.
  */

void setup(void);

void help(void);

void cleanup(void);
char *TCID = "link03";
int TST_TOTAL = 2;
#define BASENAME	"lkfile"

char fname[255];

int nlinks = 0;

char *links_arg;
option_t options[] = {
	{"N:", NULL, &links_arg},
	{NULL, NULL, NULL}
};
int main(int ac, char **av)
{
	int lc;
	struct stat buf;
	int i, links;
	char lname[255];
	tst_parse_opts(ac, av, options, &help);
	if (links_arg) {
		nlinks = atoi(links_arg);
		if (nlinks == 0) {
			tst_brkm(TBROK, NULL,
			         "nlinks is not a positive number");
		}
		if (nlinks > 1000) {
			tst_resm(TINFO,
				 "nlinks > 1000 - may get errno:%d (EMLINK)",
				 EMLINK);
		}
	}
	setup();
	for (lc = 0; TEST_LOOPING(lc); lc++) {
		tst_count = 0;
		if (nlinks)
			links = nlinks;
		else
			links = (lc % 90) + 10;
		for (i = 1; i < links; i++) {
			sprintf(lname, "%s%d", fname, i);
link(fname, lname);
			if (TEST_RETURN == -1) {
				tst_brkm(TFAIL | TTERRNO, cleanup,
					 "link(%s, %s) Failed", fname, lname);
			}
		}
		stat(cleanup, fname, &buf);
		if (buf.st_nlink != (nlink_t)links) {
			tst_resm(TFAIL, "Wrong number of links for "
			         "'%s' have %i, should be %i",
				 fname, (int)buf.st_nlink, links);
			goto unlink;
		}
		for (i = 1; i < links; i++) {
			sprintf(lname, "%s%d", fname, i);
			stat(cleanup, lname, &buf);
			if (buf.st_nlink != (nlink_t)links) {
				tst_resm(TFAIL,
				         "Wrong number of links for "
					 "'%s' have %i, should be %i",
					 lname, (int)buf.st_nlink, links);
				goto unlink;
			}
		}
		tst_resm(TPASS, "link() passed and linkcounts=%d match", links);
unlink:
		for (i = 1; i < links; i++) {
			sprintf(lname, "%s%d", fname, i);
			unlink(cleanup, lname);
		}
	}
	cleanup();
	tst_exit();
}

void help(void)
{
	printf("  -N #links : create #links hard links every iteration\n");
}

void setup(void)
{
	tst_sig(NOFORK, DEF_HANDLER, cleanup);
	TEST_PAUSE;
	tst_tmpdir();
	sprintf(fname, "%s_%d", BASENAME, getpid());
	touch(cleanup, fname, 0700, NULL);
}

void cleanup(void)
{
	tst_rmdir();
}

