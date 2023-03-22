#include "incl.h"
#define EXP_RET_VAL	-1
#define SPL_FILE	"/dev/null"
	char *desc;
};
char *TCID = "fdatasync02";

int testno;

int fd;

void setup(void);

void cleanup(void);

int setup1(void);

int setup2(void);

void cleanup2(void);

struct test_case_t tdat[] = {
	{EBADF, "invalid file descriptor", setup1, NULL},
	{EINVAL, "file descriptor to a special file", setup2, cleanup2},
};
int TST_TOTAL = sizeof(tdat) / sizeof(tdat[0]);
int main(int argc, char **argv)
{
	int lc;
	tst_parse_opts(argc, argv, NULL, NULL);
	setup();
	for (lc = 0; TEST_LOOPING(lc); lc++) {
		tst_count = 0;
		for (testno = 0; testno < TST_TOTAL; ++testno) {
			if ((tdat[testno].setup) && (tdat[testno].setup())) {
				continue;
			}
fdatasync(fd);
			if ((TEST_RETURN == EXP_RET_VAL) &&
			    (TEST_ERRNO == tdat[testno].experrno)) {
				tst_resm(TPASS, "Expected failure for %s, "
					 "errno: %d", tdat[testno].desc,
					 TEST_ERRNO);
			} else {
				tst_resm(TFAIL, "Unexpected results for %s ; "
					 "returned %ld (expected %d), errno %d "
					 "(expected %d)", tdat[testno].desc,
					 TEST_RETURN, EXP_RET_VAL,
					 TEST_ERRNO, tdat[testno].experrno);
			}
			if (tdat[testno].cleanup) {
				tdat[testno].cleanup();
			}
		}
	}
	cleanup();
	tst_exit();
}
int setup1(void)
{
	fd = -1;
	return 0;
}
int setup2(void)
{
	if ((fd = open(SPL_FILE, O_RDONLY)) == -1) {
		tst_resm(TBROK, "Failed to open %s", SPL_FILE);
		return 1;
	}
	return 0;
}
void cleanup2(void)
{
	close(NULL, fd);
}
void setup(void)
{
	tst_sig(NOFORK, DEF_HANDLER, cleanup);
	 * TEST_PAUSE contains the code to fork the test with the -c option.
	 */
	TEST_PAUSE;
}
void cleanup(void)
{
}

