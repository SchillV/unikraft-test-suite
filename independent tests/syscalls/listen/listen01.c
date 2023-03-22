#include "incl.h"
char *TCID = "listen01";
int testno;
void setup(void), setup0(void), setup1(void),
cleanup(void), cleanup0(void), cleanup1(void);
	void (*setup) (void);
	void (*cleanup) (void);
	char *desc;
} tdat[] = {
	{
	0, 0, 0, 0, -1, EBADF, setup0, cleanup0, "bad file descriptor"}, {
	0, 0, 0, 0, -1, ENOTSOCK, setup0, cleanup0, "not a socket"}, {
PF_INET, SOCK_DGRAM, 0, 0, -1, EOPNOTSUPP, setup1, cleanup1,
		    "UDP listen"},};
int TST_TOTAL = sizeof(tdat) / sizeof(tdat[0]);
int main(int argc, char *argv[])
{
	int lc;
	tst_parse_opts(argc, argv, NULL, NULL);
	setup();
	for (lc = 0; TEST_LOOPING(lc); ++lc) {
		tst_count = 0;
		for (testno = 0; testno < TST_TOTAL; ++testno) {
			tdat[testno].setup();
listen(s, tdat[testno].backlog);
			if (TEST_RETURN != tdat[testno].retval ||
			    (TEST_RETURN < 0 &&
			     TEST_ERRNO != tdat[testno].experrno)) {
				tst_resm(TFAIL, "%s ; returned"
					 " %ld (expected %d), errno %d (expected"
					 " %d)", tdat[testno].desc,
					 TEST_RETURN, tdat[testno].retval,
					 TEST_ERRNO, tdat[testno].experrno);
			} else {
				tst_resm(TPASS, "%s successful",
					 tdat[testno].desc);
			}
			tdat[testno].cleanup();
		}
	}
	cleanup();
	tst_exit();
}
void setup(void)
{
	TEST_PAUSE;
}
void cleanup(void)
{
}
void setup0(void)
{
	if (tdat[testno].experrno == EBADF)
	else if ((s = open("/dev/null", O_WRONLY)) == -1)
		tst_brkm(TBROK, cleanup, "error opening /dev/null - "
			 "errno: %s", strerror(errno));
}
void cleanup0(void)
{
	s = -1;
}
void setup1(void)
{
	s = socket(cleanup, tdat[testno].domain, tdat[testno].type,
		        tdat[testno].proto);
}
void cleanup1(void)
{
	(void)close(s);
	s = -1;
}

