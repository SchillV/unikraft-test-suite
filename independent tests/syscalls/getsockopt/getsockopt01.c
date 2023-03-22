#include "incl.h"
char *TCID = "getsockopt01";
int testno;
struct sockaddr_in sin0, fsin1;
int sinlen;
int optval;
socklen_t optlen;
void setup(void), setup0(void), setup1(void),
cleanup(void), cleanup0(void), cleanup1(void);
	int optname;
	void *optval;
	socklen_t *optlen;
	struct sockaddr *sin;
	int salen;
	void (*setup) (void);
	void (*cleanup) (void);
	char *desc;
} tdat[] = {
	{
	PF_INET, SOCK_STREAM, 0, SOL_SOCKET, SO_OOBINLINE, &optval,
		    &optlen, (struct sockaddr *)&fsin1, sizeof(fsin1),
		    -1, EBADF, setup0, cleanup0, "bad file descriptor"}
	, {
	PF_INET, SOCK_STREAM, 0, SOL_SOCKET, SO_OOBINLINE, &optval,
		    &optlen, (struct sockaddr *)&fsin1, sizeof(fsin1),
		    -1, ENOTSOCK, setup0, cleanup0, "bad file descriptor"}
	,
#ifndef UCLINUX
	{
	PF_INET, SOCK_STREAM, 0, SOL_SOCKET, SO_OOBINLINE, 0, &optlen,
		    (struct sockaddr *)&fsin1, sizeof(fsin1), -1,
		    EFAULT, setup1, cleanup1, "invalid option buffer"}
	, {
	PF_INET, SOCK_STREAM, 0, SOL_SOCKET, SO_OOBINLINE, &optval, 0,
		    (struct sockaddr *)&fsin1, sizeof(fsin1), -1,
		    EFAULT, setup1, cleanup1, "invalid optlen"}
	,
	{
	PF_INET, SOCK_STREAM, 0, 500, SO_OOBINLINE, &optval, &optlen,
		    (struct sockaddr *)&fsin1, sizeof(fsin1), -1,
		    EOPNOTSUPP, setup1, cleanup1, "invalid level"}
	, {
	PF_INET, SOCK_STREAM, 0, IPPROTO_UDP, SO_OOBINLINE, &optval,
		    &optlen, (struct sockaddr *)&fsin1, sizeof(fsin1),
		    -1, EOPNOTSUPP, setup1, cleanup1, "invalid option name"}
	, {
	PF_INET, SOCK_STREAM, 0, IPPROTO_UDP, SO_OOBINLINE, &optval,
		    &optlen, (struct sockaddr *)&fsin1, sizeof(fsin1),
		    -1, EOPNOTSUPP, setup1, cleanup1,
		    "invalid option name (UDP)"}
	, {
	PF_INET, SOCK_STREAM, 0, IPPROTO_IP, -1, &optval, &optlen,
		    (struct sockaddr *)&fsin1, sizeof(fsin1), -1,
		    ENOPROTOOPT, setup1, cleanup1, "invalid option name (IP)"}
	, {
	PF_INET, SOCK_STREAM, 0, IPPROTO_TCP, -1, &optval, &optlen,
		    (struct sockaddr *)&fsin1, sizeof(fsin1), -1,
		    ENOPROTOOPT, setup1, cleanup1, "invalid option name (TCP)"}
,};
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
getsockopt(s, tdat[testno].leve;
					tdat[testno].optname,
					tdat[testno].optval,
					tdat[testno].optlen));
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
	optlen = sizeof(optval);
}
void cleanup1(void)
{
	(void)close(s);
	s = -1;
}

