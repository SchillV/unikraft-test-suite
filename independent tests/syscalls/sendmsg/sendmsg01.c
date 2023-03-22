#include "incl.h"
char *TCID = "sendmsg01";
int testno;

char buf[1024], bigbuf[128 * 1024];

int s;

struct sockaddr_in sin1, sin2;

struct sockaddr_un sun1;

struct msghdr msgdat;

char cbuf[4096];

struct cmsghdr *control;

int controllen;

struct iovec iov[1];



void setup(void);

void setup0(void);

void setup1(void);

void setup2(void);

void setup3(void);

void setup4(void);

void setup5(void);

void setup6(void);

void setup8(void);

void cleanup(void);

void cleanup0(void);

void cleanup1(void);

void cleanup4(void);

void do_child(void);
	struct iovec *iov;
	struct msghdr *msg;
	unsigned flags;
	void (*setup) (void);
	void (*cleanup) (void);
	char *desc;
};
struct test_case_t tdat[] = {
	{.domain = PF_INET,
	 .type = SOCK_STREAM,
	 .proto = 0,
	 .iov = iov,
	 .iovcnt = 1,
	 .buf = buf,
	 .buflen = sizeof(buf),
	 .msg = &msgdat,
	 .flags = 0,
	 .to = (struct sockaddr *)&sin1,
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
	 .iov = iov,
	 .iovcnt = 1,
	 .buf = buf,
	 .buflen = sizeof(buf),
	 .msg = &msgdat,
	 .flags = 0,
	 .to = (struct sockaddr *)&sin1,
	 .tolen = sizeof(sin1),
	 .retval = -1,
	 .experrno = ENOTSOCK,
	 .setup = setup0,
	 .cleanup = cleanup0,
	 .desc = "invalid socket"}
	,
	{.domain = PF_INET,
	 .type = SOCK_DGRAM,
	 .proto = 0,
	 .iov = iov,
	 .iovcnt = 1,
	 .buf = (void *)-1,
	 .buflen = sizeof(buf),
	 .msg = &msgdat,
	 .flags = 0,
	 .to = (struct sockaddr *)&sin1,
	 .tolen = sizeof(sin1),
	 .retval = -1,
	 .experrno = EFAULT,
	 .setup = setup1,
	 .cleanup = cleanup1,
	 .desc = "invalid send buffer"}
	,
	{.domain = PF_INET,
	 .type = SOCK_STREAM,
	 .proto = 0,
	 .iov = iov,
	 .iovcnt = 1,
	 .buf = buf,
	 .buflen = sizeof(buf),
	 .msg = &msgdat,
	 .flags = 0,
	 .to = (struct sockaddr *)&sin2,
	 .tolen = sizeof(sin2),
	 .retval = 0,
	 .experrno = EFAULT,
	 .setup = setup5,
	 .cleanup = cleanup1,
	 .desc = "connected TCP"}
	,
	{.domain = PF_INET,
	 .type = SOCK_STREAM,
	 .proto = 0,
	 .iov = iov,
	 .iovcnt = 1,
	 .buf = buf,
	 .buflen = sizeof(buf),
	 .msg = &msgdat,
	 .flags = 0,
	 .to = (struct sockaddr *)&sin1,
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
	 .iov = iov,
	 .iovcnt = 1,
	 .buf = buf,
	 .buflen = sizeof(buf),
	 .msg = &msgdat,
	 .flags = 0,
	 .to = (struct sockaddr *)&sin1,
	 .tolen = 1,
	 .retval = -1,
	 .experrno = EINVAL,
	 .setup = setup1,
	 .cleanup = cleanup1,
	 .desc = "invalid to buffer length"},
	{.domain = PF_INET,
	 .type = SOCK_DGRAM,
	 .proto = 0,
	 .iov = iov,
	 .iovcnt = 1,
	 .buf = buf,
	 .buflen = sizeof(buf),
	 .msg = &msgdat,
	 .flags = 0,
	 .to = (struct sockaddr *)-1,
	 .tolen = sizeof(struct sockaddr),
	 .retval = -1,
	 .experrno = EFAULT,
	 .setup = setup1,
	 .cleanup = cleanup1,
	 .desc = "invalid to buffer"},
	{.domain = PF_INET,
	 .type = SOCK_DGRAM,
	 .proto = 0,
	 .iov = iov,
	 .iovcnt = 1,
	 .buf = bigbuf,
	 .buflen = sizeof(bigbuf),
	 .msg = &msgdat,
	 .flags = 0,
	 .to = (struct sockaddr *)&sin1,
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
	 .iov = iov,
	 .iovcnt = 1,
	 .buf = buf,
	 .buflen = sizeof(buf),
	 .msg = &msgdat,
	 .flags = 0,
	 .to = (struct sockaddr *)&sin1,
	 .tolen = sizeof(sin1),
	 .retval = -1,
	 .experrno = EPIPE,
	 .setup = setup2,
	 .cleanup = cleanup1,
	 .desc = "local endpoint shutdown"}
	,
	{.domain = PF_INET,
	 .type = SOCK_STREAM,
	 .proto = 0,
	 .iov = NULL,
	 .iovcnt = 1,
	 .buf = buf,
	 .buflen = sizeof(buf),
	 .msg = &msgdat,
	 .flags = 0,
	 .to = (struct sockaddr *)&sin1,
	 .tolen = sizeof(sin1),
	 .retval = -1,
	 .experrno = EFAULT,
	 .setup = setup1,
	 .cleanup = cleanup1,
	 .desc = "invalid iovec pointer"}
	,
	{.domain = PF_INET,
	 .type = SOCK_STREAM,
	 .proto = 0,
	 .iov = iov,
	 .iovcnt = 1,
	 .buf = buf,
	 .buflen = sizeof(buf),
	 .msg = NULL,
	 .flags = 0,
	 .to = (struct sockaddr *)&sin1,
	 .tolen = sizeof(sin1),
	 .retval = -1,
	 .experrno = EFAULT,
	 .setup = setup1,
	 .cleanup = cleanup1,
	 .desc = "invalid msghdr pointer"}
	,
	{.domain = PF_UNIX,
	 .type = SOCK_DGRAM,
	 .proto = 0,
	 .iov = iov,
	 .iovcnt = 1,
	 .buf = buf,
	 .buflen = sizeof(buf),
	 .msg = &msgdat,
	 .flags = 0,
	 .to = (struct sockaddr *)&sun1,
	 .tolen = sizeof(sun1),
	 .retval = 0,
	 .experrno = 0,
	 .setup = setup4,
	 .cleanup = cleanup4,
	 .desc = "rights passing"}
	,
	{.domain = PF_INET,
	 .type = SOCK_DGRAM,
	 .proto = 0,
	 .iov = iov,
	 .iovcnt = 1,
	 .buf = buf,
	 .buflen = sizeof(buf),
	 .msg = &msgdat,
	 .flags = MSG_OOB,
	 .to = (struct sockaddr *)&sin1,
	 .tolen = sizeof(sin1),
	 .retval = -1,
	 .experrno = EOPNOTSUPP,
	 .setup = setup1,
	 .cleanup = cleanup1,
	 .desc = "invalid flags set"}
	,
	{.domain = PF_UNIX,
	 .type = SOCK_DGRAM,
	 .proto = 0,
	 .iov = iov,
	 .iovcnt = 1,
	 .buf = buf,
	 .buflen = sizeof(buf),
	 .msg = &msgdat,
	 .flags = 0,
	 .to = (struct sockaddr *)&sun1,
	 .tolen = sizeof(sun1),
	 .retval = 0,
	 .experrno = EOPNOTSUPP,
	 .setup = setup6,
	 .cleanup = cleanup4,
	 .desc = "invalid cmsg length"}
	,
	{.domain = PF_UNIX,
	 .type = SOCK_DGRAM,
	 .proto = 0,
	 .iov = iov,
	 .iovcnt = 1,
	 .buf = buf,
	 .buflen = sizeof(buf),
	 .msg = &msgdat,
	 .flags = 0,
	 .to = (struct sockaddr *)&sun1,
	 .tolen = sizeof(sun1),
	 .retval = -1,
	 .experrno = EFAULT,
	 .setup = setup8,
	 .cleanup = cleanup4,
	 .desc = "invalid cmsg pointer"}
};
int TST_TOTAL = sizeof(tdat) / sizeof(tdat[0]);
#ifdef UCLINUX

char *argv0;
#endif
int main(int argc, char *argv[])
{
	int lc;
	tst_parse_opts(argc, argv, NULL, NULL);
#ifdef UCLINUX
	argv0 = argv[0];
	maybe_run_child(&do_child, "dd", &sfd, &ufd);
#endif
	setup();
	for (lc = 0; TEST_LOOPING(lc); ++lc) {
		tst_count = 0;
		for (testno = 0; testno < TST_TOTAL; ++testno) {
			tdat[testno].setup();
			iov[0].iov_base = tdat[testno].buf;
			iov[0].iov_len = tdat[testno].buflen;
			if (tdat[testno].type != SOCK_STREAM) {
				msgdat.msg_name = tdat[testno].to;
				msgdat.msg_namelen = tdat[testno].tolen;
			}
			msgdat.msg_iov = tdat[testno].iov;
			msgdat.msg_iovlen = tdat[testno].iovcnt;
			msgdat.msg_control = control;
			msgdat.msg_controllen = controllen;
			msgdat.msg_flags = 0;
sendmsg(s, tdat[testno].msg, tdat[testno].flags);
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

pid_t start_server(struct sockaddr_in *sin0, struct sockaddr_un *sun0)
{
	pid_t pid;
	socklen_t slen = sizeof(*sin0);
	sin0->sin_family = AF_INET;
	sin0->sin_addr.s_addr = INADDR_ANY;
	sfd = socket(PF_INET, SOCK_STREAM, 0);
	if (sfd < 0) {
		tst_brkm(TBROK, cleanup, "server socket failed: %s",
			 strerror(errno));
		return -1;
	}
	if (bind(sfd, (struct sockaddr *)sin0, sizeof(*sin0)) < 0) {
		tst_brkm(TBROK, cleanup, "server bind failed: %s",
			 strerror(errno));
		return -1;
	}
	if (listen(sfd, 10) < 0) {
		tst_brkm(TBROK, cleanup, "server listen failed: %s",
			 strerror(errno));
		return -1;
	}
	getsockname(cleanup, sfd, cleanup, sfd, struct sockaddr *)sin0, &slen);
	ufd = socket(PF_UNIX, SOCK_DGRAM, 0);
	if (ufd < 0) {
		tst_brkm(TBROK, cleanup, "server UD socket failed: %s",
			 strerror(errno));
		return -1;
	}
	if (bind(ufd, (struct sockaddr *)sun0, sizeof(*sun0))) {
		tst_brkm(TBROK, cleanup, "server UD bind failed: %s",
			 strerror(errno));
		return -1;
	}
	switch ((pid = FORK_OR_VFORK())) {
	case 0:
#ifdef UCLINUX
		if (self_exec(argv0, "dd", sfd, ufd) < 0)
			tst_brkm(TBROK, cleanup, "server self_exec failed");
#else
		do_child();
#endif
		break;
	case -1:
		tst_brkm(TBROK, cleanup, "server fork failed: %s",
			 strerror(errno));
	default:
		close(sfd);
		close(ufd);
		return pid;
	}
	exit(1);
}

void do_child(void)
{
	struct sockaddr_in fsin;
	struct sockaddr_un fsun;
	fd_set afds, rfds;
	int nfds, cc, fd;
	FD_ZERO(&afds);
	FD_SET(sfd, &afds);
	FD_SET(ufd, &afds);
	nfds = MAX(sfd + 1, ufd + 1);
	while (1) {
		socklen_t fromlen;
		memcpy(&rfds, &afds, sizeof(rfds));
		if (select(nfds, &rfds, NULL, NULL, NULL) < 0)
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
		if (FD_ISSET(ufd, &rfds)) {
			int newfd;
			fromlen = sizeof(fsun);
			newfd = accept(ufd, (struct sockaddr *)&fsun, &fromlen);
			if (newfd >= 0)
				FD_SET(newfd, &afds);
		}
		for (fd = 0; fd < nfds; ++fd) {
			if (fd != sfd && fd != ufd && FD_ISSET(fd, &rfds)) {
				cc = read(fd, buf, sizeof(buf));
				if (cc == 0 || (cc < 0 && errno != EINTR)) {
					close(fd);
					FD_CLR(fd, &afds);
				}
			}
		}
	}
}

pid_t pid;

char tmpsunpath[1024];

void setup(void)
{
	int ret = 0;
	tst_require_root();
	tst_sig(FORK, DEF_HANDLER, cleanup);
	TEST_PAUSE;
	tst_tmpdir();
	snprintf(tmpsunpath, 1024, "udsock%ld", (long)time(NULL));
	sun1.sun_family = AF_UNIX;
	strcpy(sun1.sun_path, tmpsunpath);
	 * configured, so making sure in setup that at least lo is up
	 */
	ret = system("ip link set lo up");
	if (WEXITSTATUS(ret) != 0) {
		ret = system("ifconfig lo up 127.0.0.1");
		if (WEXITSTATUS(ret) != 0) {
			tst_brkm(TBROK, cleanup,
			    "ip/ifconfig failed to bring up loop back device");
		}
	}
	pid = start_server(&sin1, &sun1);
	signal(SIGPIPE, SIG_IGN);
}

void cleanup(void)
{
	if (pid > 0)
	unlink(tmpsunpath);
	tst_rmdir();
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
	if (tdat[testno].type == SOCK_STREAM &&
	    connect(s, (struct sockaddr *)tdat[testno].to,
		    tdat[testno].tolen) < 0) {
		tst_brkm(TBROK, cleanup, "connect failed: %s", strerror(errno));
	}
}

void cleanup1(void)
{
	close(s);
	s = -1;
}

void setup2(void)
{
	if (shutdown(s, 1) < 0) {
		tst_brkm(TBROK, cleanup, "socket setup failed connect "
			 "test %d: %s", testno, strerror(errno));
	}
}

void setup3(void)
{
	s = socket(cleanup, tdat[testno].domain, tdat[testno].type,
		        tdat[testno].proto);
}

char tmpfilename[1024];

int tfd;

void setup4(void)
{
	strcpy(tmpfilename, "sockXXXXXX");
	tfd = mkstemp(tmpfilename);
	if (tfd < 0) {
		tst_brkm(TBROK, cleanup4, "socket setup failed: %s",
			 strerror(errno));
	}
	control = (struct cmsghdr *)cbuf;
	memset(cbuf, 0x00, sizeof(cbuf));
	control->cmsg_len = sizeof(struct cmsghdr) + 4;
	control->cmsg_level = SOL_SOCKET;
	control->cmsg_type = SCM_RIGHTS;
	controllen = control->cmsg_len;
}

void cleanup4(void)
{
	cleanup1();
	close(tfd);
	tfd = -1;
	control = 0;
	controllen = 0;
}

void setup5(void)
{
	s = socket(cleanup, tdat[testno].domain, tdat[testno].type,
			tdat[testno].proto);
	connect(cleanup, s, cleanup, s, struct sockaddr *)&sin1, sizeofsin1));
	 * 5-tuple than already connected
	 */
	sin2 = sin1;
	sin2.sin_port = TST_GET_UNUSED_PORT(cleanup, AF_INET, SOCK_STREAM);
}

void setup6(void)
{
	setup4();
	controllen = control->cmsg_len = sizeof(struct cmsghdr) - 4;
*/
	controllen = control->cmsg_len = 0;
}

void setup8(void)
{
	setup4();
	control = (struct cmsghdr *)-1;
}

