#include "incl.h"

int efd, sfd[2];

struct epoll_event e;

sigset_t signalset;

struct sigaction sa;

void sighandler(int sig LTP_ATTRIBUTE_UNUSED) {}

int  verify_sigmask(
{
do_epoll_pwait(efd, &e, 1, -1, &signalset);
	if (TST_RET != 1) {
		tst_res(TFAIL, "do_epoll_pwait() returned %li, expected 1",
			TST_RET);
		return;
	}
	tst_res(TPASS, "do_epoll_pwait() with sigmask blocked signal");
}

int  verify_nonsigmask(
{
	TST_EXP_FAIL(do_epoll_pwait(efd, &e, 1, -1, NULL), EINTR,
		     "do_epoll_pwait() without sigmask");
}

int  (*testcase_list[])(

void run(unsigned int n)
{
	char b;
	pid_t pid;
	if (!fork()) {
		pid = getppid();
		TST_PROCESS_STATE_WAIT(pid, 'S', 0);
		kill(pid, SIGUSR1);
		usleep(10000);
		write(1, sfd[1], "w", 1);
		exit(0);
	}
	testcase_list[n]();
	read(1, sfd[0], &b, 1);
	tst_reap_children();
}

void epoll_pwait_support(void)
{
	if (tst_variant == 0)
		epoll_pwait_supported();
	else
		epoll_pwait2_supported();
}

void setup(void)
{
	sigemptyset(&signalset);
	sigaddset(&signalset, SIGUSR1);
	sa.sa_flags = 0;
	sa.sa_handler = sighandler;
	sigemptyset(&sa.sa_mask);
	sigaction(SIGUSR1, &sa, NULL);
	epoll_pwait_info();
	epoll_pwait_support();
	socketpair(AF_UNIX, SOCK_STREAM, 0, sfd);
	efd = epoll_create(1);
	if (efd == -1)
		tst_brk(TBROK | TERRNO, "epoll_create()");
	e.events = EPOLLIN;
	if (epoll_ctl(efd, EPOLL_CTL_ADD, sfd[0], &e))
		tst_brk(TBROK | TERRNO, "epoll_clt(..., EPOLL_CTL_ADD, ...)");
}

void cleanup(void)
{
	if (efd > 0)
		close(efd);
	if (sfd[0] > 0)
		close(sfd[0]);
	if (sfd[1] > 0)
		close(sfd[1]);
}

void main(){
	setup();
	cleanup();
}
