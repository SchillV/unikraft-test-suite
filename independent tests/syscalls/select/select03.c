#include "incl.h"

fd_set readfds_reg, writefds_reg, fds_closed;

fd_set *preadfds_reg = &readfds_reg, *pwritefds_reg = &writefds_reg;

fd_set *pfds_closed = &fds_closed, *nullfds = NULL, *faulty_fds;

int fd_closed, fd[2];

int negative_nfds = -1, maxfds;

struct timeval timeout = {.tv_sec = 0, .tv_usec = 100000};

struct timeval *valid_to = &timeout, *invalid_to;

struct tcases {
	char *name;
	int *nfds;
	fd_set **readfds;
	fd_set **writefds;
	fd_set **exceptfds;
	struct timeval **timeout;
	int exp_errno;
} tests[] = {
	{ "Negative nfds", &negative_nfds, &preadfds_reg, &pwritefds_reg, &nullfds, &valid_to, EINVAL },
	{ "Invalid readfds", &maxfds, &pfds_closed, &pwritefds_reg, &nullfds, &valid_to, EBADF },
	{ "Invalid writefds", &maxfds, &preadfds_reg, &pfds_closed, &nullfds, &valid_to, EBADF },
	{ "Invalid exceptfds", &maxfds, &preadfds_reg, &pwritefds_reg, &pfds_closed, &valid_to, EBADF },
	{ "Faulty readfds", &maxfds, &faulty_fds, &pwritefds_reg, &nullfds, &valid_to, EFAULT },
	{ "Faulty writefds", &maxfds, &preadfds_reg, &faulty_fds, &nullfds, &valid_to, EFAULT },
	{ "Faulty exceptfds", &maxfds, &preadfds_reg, &pwritefds_reg, &faulty_fds, &valid_to, EFAULT },
	{ "Faulty timeout", &maxfds, &preadfds_reg, &pwritefds_reg, &nullfds, &invalid_to, EFAULT },
};

int  verify_select(unsigned int n)
{
	struct tcases *tc = &tests[n];
do_select_faulty_to(*tc->nfds, *tc->readfds, *tc->writefd;
				 *tc->exceptfds, *tc->timeout,
				 tc->timeout == &invalid_to));
	if (TST_RET != -1) {
		tst_res(TFAIL, "%s: select() passed unexpectedly with %ld",
		        tc->name, TST_RET);
		return;
	}
	if (tc->exp_errno != TST_ERR) {
		tst_res(TFAIL | TTERRNO, "%s: select()() should fail with %s",
			tc->name, tst_strerrno(tc->exp_errno));
		return;
	}
	tst_res(TPASS | TTERRNO, "%s: select() failed as expected", tc->name);
	exit(0);
}

void run(unsigned int n)
{
	int pid, status;
	pid = fork();
	if (!pid)
int 		verify_select(n);
	waitpid(pid, &status, 0);
	if (WIFEXITED(status))
		return;
	if (tst_variant == GLIBC_SELECT_VARIANT &&
	    tests[n].timeout == &invalid_to &&
	    WIFSIGNALED(status) && WTERMSIG(status) == SIGSEGV) {
		tst_res(TPASS, "%s: select() killed by signal", tests[n].name);
		return;
	}
	tst_res(TFAIL, "Child %s", tst_strstatus(status));
}

void setup(void)
{
	void *faulty_address;
	select_info();
	fd_closed = open("tmpfile1", O_CREAT | O_RDWR, 0777);
	FD_ZERO(&fds_closed);
	FD_SET(fd_closed, &fds_closed);
	pipe(fd);
	FD_ZERO(&readfds_reg);
	FD_ZERO(&writefds_reg);
	FD_SET(fd[0], &readfds_reg);
	FD_SET(fd[1], &writefds_reg);
	close(fd_closed);
	maxfds = fd[1] + 1;
	faulty_address = tst_get_bad_addr(NULL);
	invalid_to = faulty_address;
	faulty_fds = faulty_address;
}

void main(){
	setup();
	cleanup();
}
