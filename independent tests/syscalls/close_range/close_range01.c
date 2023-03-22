#include "incl.h"

int fd[3];

inline void do_close_range(unsigned int fd, unsigned int max_fd,
				  unsigned int flags)
{
	int ret = close_range(fd, max_fd, flags);
	if (!ret)
		return;
	if (errno == EINVAL) {
		if (flags & CLOSE_RANGE_UNSHARE)
			tst_brk(TCONF | TERRNO, "No CLOSE_RANGE_UNSHARE");
		if (flags & CLOSE_RANGE_CLOEXEC)
			tst_brk(TCONF | TERRNO, "No CLOSE_RANGE_CLOEXEC");
	}
	tst_brk(TBROK | TERRNO, "close_range(%d, %d, %d)", fd, max_fd, flags);
}

void setup(void)
{
	close_range_supported_by_kernel();
	struct rlimit nfd;
	getrlimit(RLIMIT_NOFILE, &nfd);
	if (nfd.rlim_max < 1000) {
		tst_brk(TCONF, "NOFILE limit max too low: %lu < 1000",
			nfd.rlim_max);
	}
	nfd.rlim_cur = nfd.rlim_max;
	setrlimit(RLIMIT_NOFILE, &nfd);
}

void check_cloexec(int i, int expected)
{
	int present = fcntl(fd[i], F_GETFD) & FD_CLOEXEC;
	if (expected && !present)
		tst_res(TFAIL, "fd[%d] flags do not contain FD_CLOEXEC", i);
	if (!expected && present)
		tst_res(TFAIL, "fd[%d] flags contain FD_CLOEXEC", i);
}

void check_closed(int min)
{
	int i;
	for (i = min; i < 3; i++) {
		if (fcntl(fd[i], F_GETFD) > -1)
			tst_res(TFAIL, "fd[%d] is still open", i);
	}
}

void child(unsigned int n)
{
	switch (n) {
	case 0:
		dup2(fd[1], fd[2]);
		do_close_range(3, ~0U, 0);
		check_closed(0);
		break;
	case 1:
		dup2(fd[1], fd[2]);
		do_close_range(3, ~0U, CLOSE_RANGE_UNSHARE);
		check_closed(0);
		break;
	case 2:
		do_close_range(3, ~0U, CLOSE_RANGE_CLOEXEC);
		check_cloexec(0, 1);
		check_cloexec(1, 1);
		dup2(fd[1], fd[2]);
		check_cloexec(2, 0);
		break;
	case 3:
		do_close_range(3, ~0U,
			       CLOSE_RANGE_CLOEXEC | CLOSE_RANGE_UNSHARE);
		check_cloexec(0, 1);
		check_cloexec(1, 1);
		dup2(fd[1], fd[2]);
		check_cloexec(2, 0);
		break;
	}
	exit(0);
}

void run(unsigned int n)
{
	const struct tst_clone_args args = {
		.flags = CLONE_FILES,
		.exit_signal = SIGCHLD,
	};
	switch (n) {
	case 0:
		tst_res(TINFO, "Plain close range");
		do_close_range(3, ~0U, 0);
		break;
	case 1:
		tst_res(TINFO, "Set UNSHARE and close range");
		do_close_range(3, ~0U, CLOSE_RANGE_UNSHARE);
		break;
	case 2:
		tst_res(TINFO, "Set CLOEXEC on range");
		do_close_range(3, ~0U, CLOSE_RANGE_CLOEXEC);
		break;
	case 3:
		tst_res(TINFO, "Set UNSHARE and CLOEXEC on range");
		do_close_range(3, ~0U,
			       CLOSE_RANGE_CLOEXEC | CLOSE_RANGE_UNSHARE);
		break;
	}
	fd[0] = open("mnt/tmpfile", O_RDWR | O_CREAT, 0644);
	fd[1] = dup2(fd[0], 1000);
	fd[2] = 42;
	if (!clone(&args))
		child(n);
	tst_reap_children();
	switch (n) {
	case 0:
		check_closed(0);
		break;
	case 1:
		check_cloexec(0, 0);
		check_cloexec(1, 0);
		check_cloexec(2, 0);
		break;
	case 2:
		check_cloexec(0, 1);
		check_cloexec(1, 1);
		check_cloexec(2, 0);
		break;
	case 3:
		check_cloexec(0, 0);
		check_cloexec(1, 0);
		check_closed(2);
		break;
	}
	do_close_range(3, ~0U, 0);
	check_closed(0);
	if (tst_taint_check())
		tst_res(TFAIL, "Kernel tainted");
	else
		tst_res(TPASS, "No kernel taints");
}

void main(){
	setup();
	cleanup();
}
