#include "incl.h"
char *TCID = "sendto01";
int testno;

char buf[1024], bigbuf[128 * 1024];

int s;

struct sockaddr_in sin1, sin2;

int sfd;
	int retval;
	int experrno;
	void (*setup) (void);
	void (*cleanup) (void);
	char *desc;
};

void setup(void);

void setup0(void);

void setup1(void);

void setup2(void);

void setup3(void);

void cleanup(void);

void cleanup0(void);

void cleanup1(void);

void do_child(void);
struct test_case_t tdat[] = {
	{.domain = PF_INET,
	 .type = SOCK_STREAM,
	 .proto = 0,
	 .buf = buf,
	 .buflen = sizeof(buf),
	 .flags = 0,
	 .to = &sin1,
	 .tolen = sizeof(sin1),
	 .retval = -1,
	 .experrno = EBADF,
	 .setup = setup0,
	 .cleanup = cleanup0,
	 .desc = "bad file descriptor"}
	,
	{.domain = 0,
	 .type = 0,
	 .proto = 0,
	 .buf = buf,
	 .buflen = sizeof(buf),
	 .flags = 0,
	 .to = &sin1,
	 .tolen = sizeof(sin1),
	 .retval = -1,
	 .experrno = ENOTSOCK,
	 .setup = setup0,
	 .cleanup = cleanup0,
	 .desc = "invalid socket"}
	,
#ifndef UCLINUX
	{.domain = PF_INET,
	 .type = SOCK_DGRAM,
	 .proto = 0,
	 .buf = (void *)-1,
	 .buflen = sizeof(buf),
	 .flags = 0,
	 .to = &sin1,
	 .tolen = sizeof(sin1),
	 .retval = -1,
	 .experrno = EFAULT,
	 .setup = setup1,
	 .cleanup = cleanup1,
	 .desc = "invalid send buffer"}
	,
#endif
	{.domain = PF_INET,
	 .type = SOCK_STREAM,
	 .proto = 0,
	 .buf = buf,
	 .buflen = sizeof(buf),
	 .flags = 0,
	 .to = &sin2,
	 .tolen = sizeof(sin2),
	 .retval = 0,
	 .experrno = EFAULT,
	 .setup = setup1,
	 .cleanup = cleanup1,
	 .desc = "connected TCP"}
	,
	{.domain = PF_INET,
	 .type = SOCK_STREAM,
	 .proto = 0,
	 .buf = buf,
	 .buflen = sizeof(buf),
	 .flags = 0,
	 .to = &sin1,
	 .tolen = sizeof(sin1),
	 .retval = -1,
	 .experrno = EPIPE,
	 .setup = setup3,
	 .cleanup = cleanup1,
	 .desc = "not connected TCP"}
	,
	{.domain = PF_INET,
	 .type = SOCK_DGRAM,
	 .proto = 0,
	 .buf = buf,
	 .buflen = sizeof(buf),
	 .flags = 0,
	 .to = &sin1,
	 .tolen = -1,
	 .retval = -1,
	 .experrno = EINVAL,
	 .setup = setup1,
	 .cleanup = cleanup1,
	 .desc = "invalid to buffer length"}
	,
#ifndef UCLINUX
	{.domain = PF_INET,
	 .type = SOCK_DGRAM,
	 .proto = 0,
	 .buf = buf,
	 .buflen = sizeof(buf),
	 .flags = 0,
	 .to = (struct sockaddr_in *)-1,
	 .tolen = sizeof(sin1),
	 .retval = -1,
	 .experrno = EFAULT,
	 .setup = setup1,
	 .cleanup = cleanup1,
	 .desc = "invalid to buffer"}
	,
#endif
	{.domain = PF_INET,
	 .type = SOCK_DGRAM,
	 .proto = 0,
	 .buf = bigbuf,
	 .buflen = sizeof(bigbuf),
	 .flags = 0,
	 .to = &sin1,
	 .tolen = sizeof(sin1),
	 .retval = -1,
	 .experrno = EMSGSIZE,
	 .setup = setup1,
	 .cleanup = cleanup1,
	 .desc = "UDP message too big"}
	,
	{.domain = PF_INET,
	 .type = SOCK_STREAM,
	 .proto = 0,
	 .buf = buf,
	 .buflen = sizeof(buf),
	 .flags = 0,
	 .to = &sin1,
	 .tolen = sizeof(sin1),
	 .retval = -1,
	 .experrno = EPIPE,
	 .setup = setup2,
	 .cleanup = cleanup1,
	 .desc = "local endpoint shutdown"}
	,
	{.domain = PF_INET,
	 .type = SOCK_DGRAM,
	 .proto = 0,
	 .buf = buf,
	 .buflen = sizeof(buf),
	 .flags = MSG_OOB,
	 .to = &sin1,
	 .tolen = sizeof(sin1),
	 .retval = -1,
	 .experrno = EOPNOTSUPP,
	 .setup = setup1,
	 .cleanup = cleanup1,
	 .desc = "invalid flags set"}
};
int TST_TOTAL = sizeof(tdat) / sizeof(tdat[0]);
#ifdef UCLINUX

char *argv0;
#endif

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
	case 0:
#ifdef UCLINUX
		if (self_exec(argv0, "d", sfd) < 0)
			tst_brkm(TBROK | TERRNO, cleanup,
				 "server self_exec failed");
#else
		do_child();
#endif
		break;
	case -1:
		tst_brkm(TBROK | TERRNO, cleanup, "server fork failed");
	default:
		(void)close(sfd);
		return pid;
	}
	exit(1);
}

void do_child(void)
{
	struct sockaddr_in fsin;
	fd_set afds, rfds;
	int nfds, cc, fd;
	FD_ZERO(&afds);
	FD_SET(sfd, &afds);
	nfds = sfd + 1;
	while (1) {
		socklen_t fromlen;
		memcpy(&rfds, &afds, sizeof(rfds));
		if (select(nfds, &rfds, NULL, NULL, NULL) < 0 && errno != EINTR)
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
		for (fd = 0; fd < nfds; ++fd) {
			if (fd != sfd && FD_ISSET(fd, &rfds)) {
				cc = read(fd, buf, sizeof(buf));
				if (cc == 0 || (cc < 0 && errno != EINTR)) {
					(void)close(fd);
					FD_CLR(fd, &afds);
				}
			}
		}
	}
}
int main(int ac, char *av[])
{
	int lc;
	tst_parse_opts(ac, av, NULL, NULL);
#ifdef UCLINUX
	argv0 = av[0];
	maybe_run_child(&do_child, "d", &sfd);
#endif
	setup();
	for (lc = 0; TEST_LOOPING(lc); ++lc) {
		tst_count = 0;
		for (testno = 0; testno < TST_TOTAL; ++testno) {
			tdat[testno].setup();
sendto(s, tdat[testno].buf, tdat[testno].bufle;
				    tdat[testno].flags,
				    (const struct sockaddr *)tdat[testno].to,
				    tdat[testno].tolen));
			if (TEST_RETURN > 0)
				TEST_RETURN = 0;
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

pid_t server_pid;

void setup(void)
{
	TEST_PAUSE;
	server_pid = start_server(&sin1);
	signal(SIGPIPE, SIG_IGN);
}

void cleanup(void)
{
	kill(server_pid, SIGKILL);
}

void setup0(void)
{
	if (tdat[testno].experrno == EBADF)
		s = 400;
	else if ((s = open("/dev/null", O_WRONLY)) == -1)
		tst_brkm(TBROK | TERRNO, cleanup, "open(/dev/null) failed");
}

void cleanup0(void)
{
	s = -1;
}

void setup1(void)
{
	s = socket(cleanup, tdat[testno].domain, tdat[testno].type,
			tdat[testno].proto);
	connect(cleanup, s, cleanup, s, const struct sockaddr *)&sin1, sizeofsin1));
}

void cleanup1(void)
{
	(void)close(s);
	s = -1;
}

void setup2(void)
{
	setup1();
	if (shutdown(s, 1) < 0)
		tst_brkm(TBROK | TERRNO, cleanup, "socket setup failed connect "
			 "test %d", testno);
}

void setup3(void)
{
	s = socket(cleanup, tdat[testno].domain, tdat[testno].type,
		        tdat[testno].proto);
}

