#include "incl.h"
char *TCID = "getsockname01";
int testno;
struct sockaddr_in sin0, fsin1;
socklen_t sinlen;
void setup(void), setup0(void), setup1(void),
cleanup(void), cleanup0(void), cleanup1(void);
	void (*setup) (void);
	void (*cleanup) (void);
	char *desc;
} tdat[] = {
	{
	PF_INET, SOCK_STREAM, 0, (struct sockaddr *)&fsin1,
		    &sinlen, -1, EBADF, setup0, cleanup0,
		    "bad file descriptor"}, {
	PF_INET, SOCK_STREAM, 0, (struct sockaddr *)&fsin1,
		    &sinlen, -1, ENOTSOCK, setup0, cleanup0,
		    "bad file descriptor"},
#ifndef UCLINUX
	{
	PF_INET, SOCK_STREAM, 0, NULL,
		    &sinlen, -1, EFAULT, setup1, cleanup1,
		    "invalid socket buffer"}, {
	PF_INET, SOCK_STREAM, 0, (struct sockaddr *)&fsin1,
		    NULL, -1, EFAULT, setup1, cleanup1,
		    "invalid aligned salen"}, {
	PF_INET, SOCK_STREAM, 0, (struct sockaddr *)&fsin1,
		    (socklen_t *) 1, -1, EFAULT, setup1, cleanup1,
		    "invalid unaligned salen"},
#endif
};
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
getsockname(s, tdat[testno].sockadd;
					 tdat[testno].salen));
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
	sin0.sin_family = AF_INET;
	sin0.sin_port = 0;
	sin0.sin_addr.s_addr = INADDR_ANY;
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
	bind(cleanup, s, cleanup, s, struct sockaddr *)&sin0, sizeofsin0));
	sinlen = sizeof(fsin1);
}
void cleanup1(void)
{
	(void)close(s);
	s = -1;
}

