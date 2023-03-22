#include "incl.h"
#define _GNU_SOURCE
TCID_DEFINE(signalfd01);
int TST_TOTAL = 1;
#ifndef HAVE_SIGNALFD
#define  USE_STUB
#endif
#if defined HAVE_SYS_SIGNALFD_H
#elif defined HAVE_LINUX_SIGNALFD_H
#define USE_OWNIMPL
#else
#define  USE_STUB
#endif
#ifndef HAVE_STRUCT_SIGNALFD_SIGINFO_SSI_SIGNO
#define USE_STUB
#endif
#ifdef USE_STUB
int main(void)
{
	tst_brkm(TCONF, NULL, "System doesn't support execution of the test");
}
#else
#if defined USE_OWNIMPL
int signalfd(int fd, const sigset_t * mask, int flags)
{
	return tst_syscall(__NR_signalfd, fd, mask, SIGSETSIZE);
}
#endif
void cleanup(void);
void setup(void);
int do_test1(uint32_t sig)
{
	int sfd_for_next;
	int sfd;
	sigset_t mask;
	pid_t pid;
	struct signalfd_siginfo fdsi;
	ssize_t s;
	sigemptyset(&mask);
	sigaddset(&mask, sig);
	if (sigprocmask(SIG_BLOCK, &mask, NULL) < 0) {
		tst_brkm(TBROK, cleanup,
			 "sigprocmask() Failed: errno=%d : %s",
			 errno, strerror(errno));
	}
signalfd(-1, &mask, 0);
	if ((sfd = TEST_RETURN) == -1) {
		tst_resm(TFAIL,
			 "signalfd() Failed, errno=%d : %s",
			 TEST_ERRNO, strerror(TEST_ERRNO));
		sfd_for_next = -1;
		return sfd_for_next;
	} else {
		tst_resm(TPASS, "signalfd is created successfully");
		sfd_for_next = sfd;
		goto out;
	}
	if (fcntl(sfd, F_SETFL, O_NONBLOCK) == -1) {
		close(sfd);
		tst_brkm(TBROK, cleanup,
			 "setting signalfd nonblocking mode failed: errno=%d : %s",
			 errno, strerror(errno));
	}
	pid = getpid();
	if (kill(pid, sig) == -1) {
		close(sfd);
		tst_brkm(TBROK, cleanup,
			 "kill(self, %s) failed: errno=%d : %s",
			 strsignal(sig), errno, strerror(errno));
	}
	s = read(sfd, &fdsi, sizeof(struct signalfd_siginfo));
	if ((s > 0) && (s != sizeof(struct signalfd_siginfo))) {
		tst_resm(TFAIL,
			 "getting incomplete signalfd_siginfo data: "
			 "actual-size=%zd, expected-size=%zu",
			 s, sizeof(struct signalfd_siginfo));
		sfd_for_next = -1;
		close(sfd);
		goto out;
	} else if (s < 0) {
		if (errno == EAGAIN) {
			tst_resm(TFAIL,
				 "signalfd_siginfo data is not delivered yet");
			sfd_for_next = -1;
			close(sfd);
			goto out;
		} else {
			close(sfd);
			tst_brkm(TBROK, cleanup,
				 "read signalfd_siginfo data failed: errno=%d : %s",
				 errno, strerror(errno));
		}
	} else if (s == 0) {
		tst_resm(TFAIL, "got EOF unexpectedly");
		sfd_for_next = -1;
		close(sfd);
		goto out;
	}
	if (fdsi.ssi_signo == sig) {
		tst_resm(TPASS, "got expected signal");
		sfd_for_next = sfd;
		goto out;
	} else {
		tst_resm(TFAIL, "got unexpected signal: signal=%d : %s",
			 fdsi.ssi_signo, strsignal(fdsi.ssi_signo));
		sfd_for_next = -1;
		close(sfd);
		goto out;
	}
out:
	return sfd_for_next;
}
void do_test2(int fd, uint32_t sig)
{
	int sfd;
	sigset_t mask;
	pid_t pid;
	struct signalfd_siginfo fdsi;
	ssize_t s;
	sigemptyset(&mask);
	sigaddset(&mask, sig);
	if (sigprocmask(SIG_BLOCK, &mask, NULL) < 0) {
		close(fd);
		tst_brkm(TBROK, cleanup,
			 "sigprocmask() Failed: errno=%d : %s",
			 errno, strerror(errno));
	}
signalfd(fd, &mask, 0);
	if ((sfd = TEST_RETURN) == -1) {
		tst_resm(TFAIL,
			 "reassignment the file descriptor by signalfd() failed, errno=%d : %s",
			 TEST_ERRNO, strerror(TEST_ERRNO));
		return;
	} else if (sfd != fd) {
		tst_resm(TFAIL,
			 "different fd is returned in reassignment: expected-fd=%d, actual-fd=%d",
			 fd, sfd);
		close(sfd);
		return;
	} else {
		tst_resm(TPASS, "signalfd is successfully reassigned");
		goto out;
	}
	pid = getpid();
	if (kill(pid, sig) == -1) {
		close(sfd);
		tst_brkm(TBROK, cleanup,
			 "kill(self, %s) failed: errno=%d : %s",
			 strsignal(sig), errno, strerror(errno));
	}
	s = read(sfd, &fdsi, sizeof(struct signalfd_siginfo));
	if ((s > 0) && (s != sizeof(struct signalfd_siginfo))) {
		tst_resm(TFAIL,
			 "getting incomplete signalfd_siginfo data: "
			 "actual-size=%zd, expected-size= %zu",
			 s, sizeof(struct signalfd_siginfo));
		goto out;
	} else if (s < 0) {
		if (errno == EAGAIN) {
			tst_resm(TFAIL,
				 "signalfd_siginfo data is not delivered yet");
			goto out;
		} else {
			close(sfd);
			tst_brkm(TBROK, cleanup,
				 "read signalfd_siginfo data failed: errno=%d : %s",
				 errno, strerror(errno));
		}
	} else if (s == 0) {
		tst_resm(TFAIL, "got EOF unexpectedly");
		goto out;
	}
	if (fdsi.ssi_signo == sig) {
		tst_resm(TPASS, "got expected signal");
		goto out;
	} else {
		tst_resm(TFAIL, "got unexpected signal: signal=%d : %s",
			 fdsi.ssi_signo, strsignal(fdsi.ssi_signo));
		goto out;
	}
out:
	return;
}
int main(int argc, char **argv)
{
	int lc;
	int sfd;
	tst_parse_opts(argc, argv, NULL, NULL);
	setup();
	for (lc = 0; TEST_LOOPING(lc); lc++) {
		tst_count = 0;
		sfd = do_test1(SIGUSR1);
		if (sfd < 0)
			continue;
		do_test2(sfd, SIGUSR2);
		close(sfd);
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
#endif

