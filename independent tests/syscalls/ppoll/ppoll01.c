#ifndef _GNU_SOURCE
#define _GNU_SOURCE
#endif

#include <errno.h>
#include <stdio.h>
#include <fcntl.h>
#include <unistd.h>
#include <poll.h>
#include <signal.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <bits/types/sigset_t.h>
#include <sys/syscall.h>

#ifndef POLLRDHUP
#define POLLRDHUP 0x2000
#endif
#define TYPE_NAME(x) .ttype = x, .desc = #x
struct test_case {
	int ttype;		   /* test type (enum) */
	const char *desc;	   /* test description (name) */
	int ret;		   /* expected ret code */
	int err;		   /* expected errno code */
	short expect_revents;	   /* expected revents value */
	unsigned int nfds;	   /* nfds ppoll parameter */
	sigset_t *sigmask;	   /* sigmask ppoll parameter */
	sigset_t *sigmask_cur;	   /* sigmask set for current process */
	struct pollfd *fds;	   /* fds ppoll parameter */
	int sigint_count;	   /* if > 0, spawn process to send SIGINT */
				   /* 'count' times to current process */
	unsigned int sigint_delay; /* delay between SIGINT signals */
};
enum test_type {
	NORMAL,
	MASK_SIGNAL,
	TIMEOUT,
	FD_ALREADY_CLOSED,
	SEND_SIGINT,
	SEND_SIGINT_RACE_TEST,
	INVALID_NFDS,
	INVALID_FDS,
};

int fd1 = -1;
sigset_t sigmask_empty, sigmask_sigint;
struct pollfd fds_good[1], fds_already_closed[1];

struct test_case tcase[] = {
	{
		TYPE_NAME(NORMAL),
		.expect_revents = POLLIN | POLLOUT,
		.ret = 1,
		.err = 0,
		.nfds = 1,
		.fds = fds_good,
	},
	{
		TYPE_NAME(MASK_SIGNAL),
		.ret = 0,
		.err = 0,
		.nfds = 0,
		.sigmask = &sigmask_sigint,
		.fds = fds_good,
		.sigint_count = 4,
		.sigint_delay = 100000,
	},
	{
		TYPE_NAME(TIMEOUT),
		.ret = 0,
		.err = 0,
		.nfds = 0,
		.fds = fds_good,
	},
	{
		TYPE_NAME(FD_ALREADY_CLOSED),
		.expect_revents = POLLNVAL,
		.ret = 1,
		.err = 0,
		.nfds = 1,
		.fds = fds_already_closed,
	},
	{
		TYPE_NAME(SEND_SIGINT),
		.ret = -1,
		.err = EINTR,
		.nfds = 0,
		.fds = fds_good,
		.sigint_count = 40,
		.sigint_delay = 100000,
	},
	{
		TYPE_NAME(SEND_SIGINT_RACE_TEST),
		.ret = -1,
		.err = EINTR,
		.nfds = 0,
		.sigmask = &sigmask_empty,
		.sigmask_cur = &sigmask_sigint,
		.fds = fds_good,
		.sigint_count = 1,
		.sigint_delay = 0,
	},
	{
		TYPE_NAME(INVALID_NFDS),
		.ret = -1,
		.err = EINVAL,
		.nfds = -1,
		.fds = fds_good,
	},
	{
		TYPE_NAME(INVALID_FDS),
		.ret = -1,
		.err = EFAULT,
		.nfds = 1,
		.fds = (struct pollfd *) -1,
	},
};

void sighandler(int sig)
{
}

void setup(void)
{
	int fd2;
	signal(SIGINT, sighandler);
	if (sigemptyset(&sigmask_empty) == -1)
		printf("sigemptyset, error number %d\n", errno);
	if (sigemptyset(&sigmask_sigint) == -1)
		printf("sigemptyset, erorr number %d\n", errno);
	if (sigaddset(&sigmask_sigint, SIGINT) == -1)
		printf("sigaddset, error number %d\n", errno);
	fd1 = open("testfile1", O_CREAT | O_RDWR, S_IRUSR | S_IWUSR);
	fds_good[0].fd = fd1;
	fds_good[0].events = POLLIN | POLLPRI | POLLOUT | POLLRDHUP;
	fds_good[0].revents = 0;
	fd2 = open("testfile2", O_CREAT | O_RDWR, S_IRUSR | S_IWUSR);
	fds_already_closed[0].fd = fd2;
	fds_already_closed[0].events = POLLIN | POLLPRI | POLLOUT | POLLRDHUP;
	fds_already_closed[0].revents = 0;
	close(fd2);
}

void cleanup(void)
{
	if (fd1 != -1)
		close(fd1);
}

int do_test(unsigned int i)
{
	pid_t pid = 0;
	int sys_ret, sys_errno = 0, dummy;
	struct test_case *tc = &tcase[i];
	printf("[I] case %s\n", tc->desc);
	if (tc->sigmask_cur) {
	    if (sigprocmask(SIG_SETMASK, tc->sigmask_cur, NULL) == -1){
			printf("sigprocmask\n");
			return 0;
		}
	}
	errno = 0;
	sys_ret = ppoll(tc->fds, tc->nfds, tc->sigmask, (_NSIG / 8));
	sys_errno = errno;
	if (tc->sigmask_cur) {
		if (sigprocmask(SIG_SETMASK, &sigmask_empty, NULL) == -1){
			printf("sigprocmask\n");
			return 0;
		}
	}
	if (pid > 0) {
		kill(pid, SIGTERM);
		wait(&dummy);
	}
	if (tc->expect_revents) {
		if (tc->fds[0].revents == tc->expect_revents){
			printf("revents=0x%04x\n", tc->expect_revents);
			return 1;
		} else {
			printf("revents=0x%04x, expected=0x%04x\n", tc->fds[0].revents, tc->expect_revents);
			return 0;
		}
	}
	if (tc->ret >= 0 && tc->ret == sys_ret) {
		printf("ret: %d\n", sys_ret);
		return 1;
	} else if (tc->ret == -1 && sys_ret == -1 && sys_errno == tc->err) {
		printf("ret: %d, errno: %d\n", sys_ret, sys_errno);
		return 1;
	} else {
		printf("ret: %d, exp: %d, ret_errno: %d, exp_errno: %d\n", sys_ret, tc->ret, sys_errno, tc->err);
		return 0;
	}
}

void main(){
	setup();
	int ok=1;
	for(int i = 0; i<8;i++)
		ok *= do_test(i);
	if(ok)
		printf("test succeeded\n");
	cleanup();
}
