#include "incl.h"
char *TCID = "recvmsg01";
int testno;
char buf[1024], cbuf[1024];
struct sockaddr_in sin1, from;
struct sockaddr_un sun1;
struct msghdr msgdat;
struct cmsghdr *control = 0;
int controllen = 0;
struct iovec iov[1];


void setup(void);
void setup0(void);
void setup1(void);
void setup2(void);
void setup3(void);
void setup4(void);
void cleanup(void);
void cleanup0(void);
void cleanup1(void);
void cleanup2(void);
void do_child(void);
void sender(int);
pid_t start_server(struct sockaddr_in *, struct sockaddr_un *);
	struct iovec *iov;
	int iovcnt;
	struct msghdr *msg;
	unsigned flags;
	void (*setup) (void);
	void (*cleanup) (void);
	char *desc;
} tdat[] = {
	{
	PF_INET, SOCK_STREAM, 0, iov, 1, buf, sizeof(buf), &msgdat, 0,
		    (struct sockaddr *)&from, sizeof(from),
		    -1, EBADF, setup0, cleanup0, "bad file descriptor"}
	,
	{
	0, 0, 0, iov, 1, (void *)buf, sizeof(buf), &msgdat, 0,
		    (struct sockaddr *)&from, sizeof(from),
		    -1, ENOTSOCK, setup0, cleanup0, "invalid socket"}
	,
	{
	PF_INET, SOCK_STREAM, 0, iov, 1, (void *)buf, sizeof(buf),
		    &msgdat, 0, (struct sockaddr *)-1, sizeof(from), 0,
		    ENOTSOCK, setup1, cleanup1, "invalid socket buffer"}
	,
	{
	PF_INET, SOCK_STREAM, 0, iov, 1, (void *)buf, sizeof(buf),
		    &msgdat, -1, (struct sockaddr *)&from, -1, -1,
		    EINVAL, setup1, cleanup1, "invalid socket length"},
	{
	PF_INET, SOCK_STREAM, 0, iov, 1, (void *)-1, sizeof(buf),
		    &msgdat, 0, (struct sockaddr *)&from, sizeof(from),
		    -1, EFAULT, setup1, cleanup1, "invalid recv buffer"}
	,
	{
	PF_INET, SOCK_STREAM, 0, 0, 1, (void *)buf, sizeof(buf),
		    &msgdat, 0, (struct sockaddr *)&from, sizeof(from),
		    -1, EFAULT, setup1, cleanup1, "invalid iovec buffer"}
	,
	{
	PF_INET, SOCK_STREAM, 0, iov, -1, (void *)buf, sizeof(buf),
		    &msgdat, 0, (struct sockaddr *)&from, sizeof(from),
		    -1, EMSGSIZE, setup1, cleanup1, "invalid iovec count"}
	,
	{
	PF_UNIX, SOCK_STREAM, 0, iov, 1, (void *)buf, sizeof(buf),
		    &msgdat, 0, (struct sockaddr *)&from, sizeof(from),
		    0, 0, setup2, cleanup2, "rights reception"}
	,
	{
	PF_INET, SOCK_STREAM, 0, iov, 1, (void *)buf, sizeof(buf),
		    &msgdat, MSG_OOB, (struct sockaddr *)&from,
		    sizeof(from), -1, EINVAL, setup1, cleanup1,
		    "invalid MSG_OOB flag set"}
	,
	{
	PF_INET, SOCK_STREAM, 0, iov, 1, (void *)buf, sizeof(buf),
		    &msgdat, MSG_ERRQUEUE, (struct sockaddr *)&from,
		    sizeof(from), -1, EAGAIN, setup1, cleanup1,
		    "invalid MSG_ERRQUEUE flag set"}
	,
	{
	PF_UNIX, SOCK_STREAM, 0, iov, 1, (void *)buf, sizeof(buf),
		    &msgdat, 0, (struct sockaddr *)&from, sizeof(from),
		    0, EINVAL, setup3, cleanup2, "invalid cmsg length"}
	,
	{
	PF_UNIX, SOCK_STREAM, 0, iov, 1, (void *)buf, sizeof(buf),
		    &msgdat, 0, (struct sockaddr *)&from, sizeof(from),
		    0, 0, setup4, cleanup2, "large cmesg length"}
,};
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
			if ((tst_kvercmp(3, 17, 0) < 0)
			    && (tdat[testno].flags & MSG_ERRQUEUE)
			    && (tdat[testno].type & SOCK_STREAM)) {
				tst_resm(TCONF, "skip MSG_ERRQUEUE test, "
						"it's supported from 3.17");
				continue;
			}
			tdat[testno].setup();
			iov[0].iov_base = tdat[testno].buf;
			iov[0].iov_len = tdat[testno].buflen;
			msgdat.msg_name = tdat[testno].from;
			msgdat.msg_namelen = tdat[testno].fromlen;
			msgdat.msg_iov = tdat[testno].iov;
			msgdat.msg_iovlen = tdat[testno].iovcnt;
			msgdat.msg_control = control;
			msgdat.msg_controllen = controllen;
			msgdat.msg_flags = 0;
recvmsg(s, tdat[testno].msg, tdat[testno].flags);
			if (TEST_RETURN >= 0)
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
char tmpsunpath[1024];
void setup(void)
{
	int tfd;
	TEST_PAUSE;
	tst_tmpdir();
	(void)strcpy(tmpsunpath, "udsockXXXXXX");
	tfd = mkstemp(tmpsunpath);
	close(tfd);
	unlink(tmpsunpath);
	sun1.sun_family = AF_UNIX;
	(void)strcpy(sun1.sun_path, tmpsunpath);
	signal(SIGPIPE, SIG_IGN);
	pid = start_server(&sin1, &sun1);
}
void cleanup(void)
{
	if (pid > 0) {
		wait(NULL);
	}
	if (tmpsunpath[0] != '\0')
		(void)unlink(tmpsunpath);
	tst_rmdir();
}
void setup0(void)
{
	if (tdat[testno].experrno == EBADF)
	else if ((s = open("/dev/null", O_WRONLY)) == -1)
		tst_brkm(TBROK | TERRNO, cleanup, "open(/dev/null) failed");
}
void cleanup0(void)
{
	s = -1;
}
void setup1(void)
{
	fd_set rdfds;
	struct timeval timeout;
	int n;
	s = socket(cleanup, tdat[testno].domain, tdat[testno].type,
			tdat[testno].proto);
	if (tdat[testno].type == SOCK_STREAM) {
		if (tdat[testno].domain == PF_INET) {
			connect(cleanup, s, cleanup, s, struct sockaddr *)&sin1,
				     sizeof(sin1));
			FD_ZERO(&rdfds);
			FD_SET(s, &rdfds);
			timeout.tv_sec = 2;
			timeout.tv_usec = 0;
			n = select(s + 1, &rdfds, 0, 0, &timeout);
			if (n != 1 || !FD_ISSET(s, &rdfds))
				tst_brkm(TBROK, cleanup,
					 "client setup1 failed - no message ready in 2 sec");
		} else if (tdat[testno].domain == PF_UNIX) {
			connect(cleanup, s, cleanup, s, struct sockaddr *)&sun1,
				     sizeof(sun1));
		}
	}
}
void setup2(void)
{
	setup1();
	if (write(s, "R", 1) < 0)
		tst_brkm(TBROK | TERRNO, cleanup, "test setup failed: write:");
	control = (struct cmsghdr *)cbuf;
	controllen = control->cmsg_len = sizeof(cbuf);
}
void setup3(void)
{
	setup2();
	controllen = sizeof(struct cmsghdr) - 1;
}
void setup4(void)
{
	setup2();
	controllen = 128 * 1024;
}
void cleanup1(void)
{
	(void)close(s);
	close(ufd);
	close(sfd);
	s = -1;
}
void cleanup2(void)
{
	close(ufd);
	close(sfd);
	(void)close(s);
	s = -1;
	if (passed_fd >= 0)
		(void)close(passed_fd);
	passed_fd = -1;
	control = 0;
	controllen = 0;
}
pid_t start_server(struct sockaddr_in *ssin, struct sockaddr_un *ssun)
{
	pid_t pid;
	socklen_t slen = sizeof(*ssin);
	ssin->sin_family = AF_INET;
	ssin->sin_addr.s_addr = INADDR_ANY;
	sfd = socket(PF_INET, SOCK_STREAM, 0);
	if (sfd < 0) {
		tst_brkm(TBROK | TERRNO, cleanup, "server socket failed");
		return -1;
	}
	if (bind(sfd, (struct sockaddr *)ssin, sizeof(*ssin)) < 0) {
		tst_brkm(TBROK | TERRNO, cleanup, "server bind failed");
		return -1;
	}
	if (listen(sfd, 10) < 0) {
		tst_brkm(TBROK | TERRNO, cleanup, "server listen failed");
		return -1;
	}
	getsockname(cleanup, sfd, cleanup, sfd, struct sockaddr *)ssin, &slen);
	ufd = socket(PF_UNIX, SOCK_STREAM, 0);
	if (ufd < 0) {
		tst_brkm(TBROK | TERRNO, cleanup, "server UD socket failed");
		return -1;
	}
	if (bind(ufd, (struct sockaddr *)ssun, sizeof(*ssun))) {
		tst_brkm(TBROK | TERRNO, cleanup, "server UD bind failed");
		return -1;
	}
	if (listen(ufd, 10) < 0) {
		tst_brkm(TBROK | TERRNO, cleanup, "server UD listen failed");
		return -1;
	}
	switch ((pid = FORK_OR_VFORK())) {
#ifdef UCLINUX
		if (self_exec(argv0, "dd", sfd, ufd) < 0)
			tst_brkm(TBROK | TERRNO, cleanup,
				 "server self_exec failed");
#else
		do_child();
#endif
		break;
	case -1:
		tst_brkm(TBROK | TERRNO, cleanup, "server fork failed");
		(void)close(sfd);
		(void)close(ufd);
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
		if (select(nfds, &rfds, NULL, NULL,
			   NULL) < 0) {
			if (errno != EINTR) {
				perror("server select");
				exit(1);
			}
			continue;
		}
		if (FD_ISSET(sfd, &rfds)) {
			int newfd;
			fromlen = sizeof(fsin);
			newfd = accept(sfd, (struct sockaddr *)&fsin, &fromlen);
			if (newfd >= 0) {
				FD_SET(newfd, &afds);
				nfds = MAX(nfds, newfd + 1);
				(void)write(newfd, "hoser\n", 6);
			}
		}
		if (FD_ISSET(ufd, &rfds)) {
			int newfd;
			fromlen = sizeof(fsun);
			newfd = accept(ufd, (struct sockaddr *)&fsun, &fromlen);
			if (newfd >= 0) {
				FD_SET(newfd, &afds);
				nfds = MAX(nfds, newfd + 1);
			}
		}
		for (fd = 0; fd < nfds; ++fd)
			if (fd != sfd && fd != ufd && FD_ISSET(fd, &rfds)) {
				char rbuf[1024];
				cc = read(fd, rbuf, sizeof(rbuf));
				if (cc && rbuf[0] == 'R')
					sender(fd);
				if (cc == 0 || (cc < 0 && errno != EINTR)) {
					(void)close(fd);
					FD_CLR(fd, &afds);
				}
			}
	}
}
#define TM	"from recvmsg01 server"
void sender(int fd)
{
	struct msghdr mh;
	struct cmsghdr *control;
	char tmpfn[1024], snd_cbuf[1024];
	int tfd;
	(void)strcpy(tmpfn, "smtXXXXXX");
	tfd = mkstemp(tmpfn);
	if (tfd < 0)
		return;
	memset(&mh, 0x00, sizeof(mh));
	control = (struct cmsghdr *)snd_cbuf;
	memset(control, 0x00, sizeof(struct cmsghdr));
	control->cmsg_len = sizeof(struct cmsghdr) + 4;
	control->cmsg_level = SOL_SOCKET;
	control->cmsg_type = SCM_RIGHTS;
	iov[0].iov_base = TM;
	iov[0].iov_len = sizeof(TM);
	mh.msg_iov = iov;
	mh.msg_iovlen = 1;
	mh.msg_flags = 0;
	mh.msg_control = control;
	mh.msg_controllen = control->cmsg_len;
	(void)sendmsg(fd, &mh, 0);
	(void)close(tfd);
	(void)unlink(tmpfn);
}

