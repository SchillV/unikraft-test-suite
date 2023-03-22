#include "incl.h"
struct sockaddr_in sin0, sin1, fsin1;
int devnull_fd;
int socket_fd;
int udp_fd;

struct test_case {
	char *desc;
} tcases[] = {
	{
		PF_INET, SOCK_STREAM, 0, &invalid_socketfd,
		(struct sockaddr *)&fsin1, sizeof(fsin1), EBADF,
		"bad file descriptor"
	},
	{
		PF_INET, SOCK_STREAM, 0, &devnull_fd, (struct sockaddr *)&fsin1,
		sizeof(fsin1), ENOTSOCK, "fd is not socket"
	},
	{
		PF_INET, SOCK_STREAM, 0, &socket_fd, (struct sockaddr *)3,
		sizeof(fsin1), EINVAL, "invalid socket buffer"
	},
	{
		PF_INET, SOCK_STREAM, 0, &socket_fd, (struct sockaddr *)&fsin1,
		1, EINVAL, "invalid salen"
	},
	{
		PF_INET, SOCK_STREAM, 0, &socket_fd, (struct sockaddr *)&fsin1,
		sizeof(fsin1), EINVAL, "no queued connections"
	},
	{
		PF_INET, SOCK_STREAM, 0, &udp_fd, (struct sockaddr *)&fsin1,
		sizeof(fsin1), EOPNOTSUPP, "UDP accept"
	},
};

void test_setup(void)
{
	sin0.sin_family = AF_INET;
	sin0.sin_port = 0;
	sin0.sin_addr.s_addr = INADDR_ANY;
	devnull_fd = open("/dev/null", O_WRONLY);
	socket_fd = socket(PF_INET, SOCK_STREAM, 0);
	bind(socket_fd, socket_fd, struct sockaddr *)&sin0, sizeofsin0));
	sin1.sin_family = AF_INET;
	sin1.sin_port = 0;
	sin1.sin_addr.s_addr = INADDR_ANY;
	udp_fd = socket(PF_INET, SOCK_DGRAM, 0);
	bind(udp_fd, udp_fd, struct sockaddr *)&sin1, sizeofsin1));
}

void test_cleanup(void)
{
	close(devnull_fd);
	close(socket_fd);
	close(udp_fd);
}
int  verify_accept(unsigned int nr)
{
	struct test_case *tcase = &tcases[nr];
	TST_EXP_FAIL2(accept(*tcase->fd, tcase->sockaddr, &tcase->salen),
	             tcase->experrno, "%s", tcase->desc);
}

