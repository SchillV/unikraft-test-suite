#include "incl.h"
char *TCID = "connect01";
int testno;
struct sockaddr_in sin1, sin2, sin3, sin4;

void setup(void), setup0(void), setup1(void), setup2(void),
cleanup(void), cleanup0(void), cleanup1(void), do_child(void);

pid_t start_server(struct sockaddr_in *);
	void (*setup) (void);
	void (*cleanup) (void);
	char *desc;
} tdat[] = {
	{
	PF_INET, SOCK_STREAM, 0, (struct sockaddr *)&sin1,
		    sizeof(struct sockaddr_in), -1, EBADF, setup0,
		    cleanup0, "bad file descriptor"},
#ifndef UCLINUX
	{
	PF_INET, SOCK_STREAM, 0, (struct sockaddr *)-1,
		    sizeof(struct sockaddr_in), -1, EFAULT, setup1,
		    cleanup1, "invalid socket buffer"},
#endif
	{
	PF_INET, SOCK_STREAM, 0, (struct sockaddr *)&sin1,
		    3, -1, EINVAL, setup1, cleanup1, "invalid salen"}, {
	0, 0, 0, (struct sockaddr *)&sin1,
		    sizeof(sin1), -1, ENOTSOCK, setup0, cleanup0,
		    "invalid socket"}
	, {
	PF_INET, SOCK_STREAM, 0, (struct sockaddr *)&sin1,
		    sizeof(sin1), -1, EISCONN, setup2, cleanup1,
		    "already connected"}
	, {
	PF_INET, SOCK_STREAM, 0, (struct sockaddr *)&sin2,
		    sizeof(sin2), -1, ECONNREFUSED, setup1, cleanup1,
		    "connection refused"}
	, {
	PF_INET, SOCK_STREAM, 0, (struct sockaddr *)&sin4,
		    sizeof(sin4), -1, EAFNOSUPPORT, setup1, cleanup1,
		    "invalid address family"}
,};
int TST_TOTAL = sizeof(tdat) / sizeof(tdat[0]);
#ifdef UCLINUX

char *argv0;
#endif

int sys_connect(int sockfd, const struct sockaddr *addr,
		socklen_t addrlen)
{
	return tst_syscall(__NR_connect, sockfd, addr, addrlen);
}
#define connect(sockfd, addr, addrlen) sys_connect(sockfd, addr, addrlen)
int main(int argc, char *argv[])
{
	int lc;
	tst_parse_opts(argc, argv, NULL, NULL);
#ifdef UCLINUX
	argv0 = argv[0];
	maybe_run_child(&do_child, "d", &sfd);
#endif
	setup();
	for (lc = 0; TEST_LOOPING(lc); ++lc) {
		tst_count = 0;
		for (testno = 0; testno < TST_TOTAL; ++testno) {
			tdat[testno].setup();
conne;
			     (s, tdat[testno].sockaddr, tdat[testno].salen));
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
pid_t pid;
void setup(void)
{
	pid = start_server(&sin1);
	sin2.sin_family = AF_INET;
	sin2.sin_port = TST_GET_UNUSED_PORT(NULL, AF_INET, SOCK_STREAM);
	sin2.sin_addr.s_addr = INADDR_ANY;
	sin3.sin_family = AF_INET;
	sin3.sin_port = 0;
	sin3.sin_addr.s_addr = htonl(0x0AFFFEFD);
	sin4.sin_port = 0;
	sin4.sin_addr.s_addr = htonl(0x0AFFFEFD);
}
void cleanup(void)
{
	(void)kill(pid, SIGKILL);
}
void setup0(void)
{
	if (tdat[testno].experrno == EBADF)
	else if ((s = open("/dev/null", O_WRONLY)) == -1)
		tst_brkm(TBROK | TERRNO, cleanup, "open(/dev/null) failed");
}
void cleanup0(void)
{
	close(s);
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
void setup2(void)
{
	connect(cleanup, s, cleanup, s, const struct sockaddr *)&sin1, sizeofsin1));
}
pid_t start_server(struct sockaddr_in *sin0)
{
	pid_t pid;
	socklen_t slen = sizeof(*sin0);
	sin0->sin_family = AF_INET;
	sin0->sin_addr.s_addr = INADDR_ANY;
	sfd = socket(PF_INET, SOCK_STREAM, 0);
	if (sfd < 0) {
		tst_brkm(TBROK | TERRNO, cleanup, "server socket failed");
		return -1;
	}
	if (bind(sfd, (struct sockaddr *)sin0, sizeof(*sin0)) < 0) {
		tst_brkm(TBROK | TERRNO, cleanup, "server bind failed");
		return -1;
	}
	if (listen(sfd, 10) < 0) {
		tst_brkm(TBROK | TERRNO, cleanup, "server listen failed");
		return -1;
	}
	getsockname(cleanup, sfd, cleanup, sfd, struct sockaddr *)sin0, &slen);
	switch ((pid = FORK_OR_VFORK())) {
#ifdef UCLINUX
		self_exec(argv0, "d", sfd);
#else
		do_child();
#endif
		break;
	case -1:
		tst_brkm(TBROK | TERRNO, cleanup, "server fork failed");
		(void)close(sfd);
		return pid;
	}
	return -1;
}
void do_child(void)
{
	struct sockaddr_in fsin;
	fd_set afds, rfds;
	int nfds, cc, fd;
	char c;
	FD_ZERO(&afds);
	FD_SET(sfd, &afds);
	nfds = sfd + 1;
	while (1) {
		socklen_t fromlen;
		memcpy(&rfds, &afds, sizeof(rfds));
		if (select(nfds, &rfds, NULL, NULL,
			   NULL) < 0)
			if (errno != EINTR)
				exit(1);
		if (FD_ISSET(sfd, &rfds)) {
			int newfd;
			fromlen = sizeof(fsin);
			newfd = accept(sfd, (struct sockaddr *)&fsin, &fromlen);
			if (newfd >= 0) {
				FD_SET(newfd, &afds);
				nfds = MAX(nfds, newfd + 1);
			}
		}
		for (fd = 0; fd < nfds; ++fd)
			if (fd != sfd && FD_ISSET(fd, &rfds)) {
				if ((cc = read(fd, &c, 1)) == 0) {
					(void)close(fd);
					FD_CLR(fd, &afds);
				}
			}
	}
}

