#include "incl.h"

struct sockaddr_in addr;

int optval;

	int optname;
	void *optval;
	int optlen;
	char *desc;
} testcase_list[] = {
	{-1, -1, -1, SOL_SOCKET, SO_OOBINLINE, &optval, sizeof(optval),
		EBADF, "invalid file descriptor"},
	{-1, -1, -1, SOL_SOCKET, SO_OOBINLINE, &optval, sizeof(optval),
		ENOTSOCK, "non-socket file descriptor"},
	{PF_INET, SOCK_STREAM, 0, SOL_SOCKET, SO_OOBINLINE, NULL,
		sizeof(optval), EFAULT, "invalid option buffer"},
	{PF_INET, SOCK_STREAM, 0, SOL_SOCKET, SO_OOBINLINE, &optval, 0,
		EINVAL, "invalid optlen"},
	{PF_INET, SOCK_STREAM, 0, 500, SO_OOBINLINE, &optval, sizeof(optval),
		ENOPROTOOPT, "invalid level"},
	{PF_INET, SOCK_STREAM, 0, IPPROTO_UDP, SO_OOBINLINE, &optval,
		sizeof(optval), ENOPROTOOPT, "invalid option name (UDP)"},
	{PF_INET, SOCK_STREAM, 0, IPPROTO_IP, -1, &optval, sizeof(optval),
		ENOPROTOOPT, "invalid option name (IP)"},
	{PF_INET, SOCK_STREAM, 0, IPPROTO_TCP, -1, &optval, sizeof(optval),
		ENOPROTOOPT, "invalid option name (TCP)"}
};

void setup(void)
{
	addr.sin_family = AF_INET;
	addr.sin_port = 0;
	addr.sin_addr.s_addr = INADDR_ANY;
}

void run(unsigned int n)
{
	struct test_case *tc = testcase_list + n;
	int tmpfd, fd;
	tst_res(TINFO, "Testing %s", tc->desc);
	if (tc->domain == -1) {
		tmpfd = fd = open("/dev/null", O_WRONLY);
	} else {
		tmpfd = fd = socket(tc->domain, tc->type, tc->proto);
		bind(fd, fd, struct sockaddr *)&addr, sizeofaddr));
	}
	if (tc->experrno == EBADF)
		close(tmpfd);
setsockopt(fd, tc->level, tc->optname, tc->optval, tc->optlen);
	if (tc->experrno != EBADF)
		close(fd);
	if (TST_RET == 0) {
		tst_res(TFAIL, "setsockopt() succeeded unexpectedly");
		return;
	}
	if (TST_RET != -1) {
		tst_res(TFAIL | TTERRNO,
			"Invalid setsockopt() return value %ld", TST_RET);
		return;
	}
	if (TST_ERR != tc->experrno) {
		tst_res(TFAIL | TTERRNO,
			"setsockopt() returned unexpected error");
		return;
	}
	tst_res(TPASS | TTERRNO, "setsockopt() returned the expected error");
}

