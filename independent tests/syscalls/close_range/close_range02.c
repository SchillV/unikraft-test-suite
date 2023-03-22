#include "incl.h"

int try_close_range(int fd, int flags)
{
	int res;
close_range(fd, fd, flags);
	if (TST_RET == -1 && TST_ERR == EINVAL)
		res = TCONF;
	else if (TST_RET == -1)
		res = TFAIL;
	else
		res = TPASS;
	return res;
}

void run(unsigned int n)
{
	const struct tst_clone_args args = {
		.flags = CLONE_FILES,
		.exit_signal = SIGCHLD,
	};
	int fd = -1, res;
	switch (n) {
	case 0:
		fd = open("/", O_PATH);
		dup2(fd, 100);
		TST_EXP_PASS(close_range(fd, 100, 0),
			     "close_range(%d, 100, 0)", fd);
		TST_EXP_FAIL(fcntl(fd, F_GETFD), EBADF,
			     "fcntl(%d, F_GETFD)", fd);
		TST_EXP_FAIL(fcntl(100, F_GETFD), EBADF);
		break;
	case 1:
		TST_EXP_FAIL(close_range(4, 3, 0), EINVAL);
		break;
	case 2:
		TST_EXP_FAIL(close_range(3, ~0U, ~0U), EINVAL);
		break;
	case 3:
		TST_EXP_PASS(close_range(~0U, ~0U, 0));
		break;
	case 4:
		fd = open("/", O_PATH);
		res = try_close_range(fd, CLOSE_RANGE_CLOEXEC);
		tst_res(res | TTERRNO,
			"close_range(%d, %d, CLOSE_RANGE_CLOEXEC)", fd, fd);
		if (res != TPASS)
			break;
		TST_EXP_FD_SILENT(fcntl(fd, F_GETFD), "fcntl(%d, F_GETFD)", fd);
		if (TST_RET & FD_CLOEXEC)
			tst_res(TPASS, "FD_CLOEXEC was set on %d", fd);
		else
			tst_res(TFAIL, "FD_CLOEXEC not set on %d", fd);
		break;
	case 5:
		fd = open("/", O_PATH);
		if (!clone(&args)) {
			res = try_close_range(fd, CLOSE_RANGE_UNSHARE);
			tst_res(res | TTERRNO,
				"close_range(%d, %d, CLOSE_RANGE_UNSHARE)",
				fd, fd);
			if (res != TPASS)
				exit(0);
			TST_EXP_FAIL(fcntl(fd, F_GETFD), EBADF,
				     "fcntl(%d, F_GETFD)", fd);
			exit(0);
		}
		tst_reap_children();
		TST_EXP_PASS(fcntl(fd, F_GETFD), "%d is open", fd);
	}
	if (fd > -1)
		TST_EXP_PASS_SILENT(close_range(fd, fd, 0),
				    "close_range(%d, %d, 0)", fd, fd);
}

void main(){
	setup();
	cleanup();
}
