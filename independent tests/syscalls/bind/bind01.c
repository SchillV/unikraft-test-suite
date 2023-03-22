#include "incl.h"
int inet_socket;
int dev_null;
struct sockaddr_in sin1, sin2, sin3;
struct sockaddr_un sun;

struct test_case {
	int *socket_fd;
	struct sockaddr *sockaddr;
	socklen_t salen;
	int retval;
	int experrno;
	char *desc;
} tcases[] = {
	{ &inet_socket, (struct sockaddr *)&sin1, 3, -1,
	  EINVAL, "invalid salen" },
	{ &dev_null, (struct sockaddr *)&sin1, sizeof(sin1), -1,
	  ENOTSOCK, "invalid socket" },
	{ &inet_socket, (struct sockaddr *)&sin2, sizeof(sin2), 0,
	  0, "INADDR_ANYPORT"},
	{ &inet_socket, (struct sockaddr *)&sun, sizeof(sun), -1,
	  EAFNOSUPPORT, "UNIX-domain of current directory" },
	{ &inet_socket, (struct sockaddr *)&sin3, sizeof(sin3), -1,
	  EADDRNOTAVAIL, "non-local address" },
};
int  verify_bind(unsigned int nr)
{
	struct test_case *tcase = &tcases[nr];
	if (tcase->experrno) {
		TST_EXP_FAIL(bind(*tcase->socket_fd, tcase->sockaddr, tcase->salen),
		             tcase->experrno, "%s", tcase->desc);
	} else {
		TST_EXP_PASS(bind(*tcase->socket_fd, tcase->sockaddr, tcase->salen),
		             "%s", tcase->desc);
		close(inet_socket);
		inet_socket = socket(PF_INET, SOCK_STREAM, 0);
	}
}
void test_setup(void)
{
	sin1.sin_family = AF_INET;
	sin1.sin_port = TST_GET_UNUSED_PORT(AF_INET, SOCK_STREAM);
	sin1.sin_addr.s_addr = INADDR_ANY;
	sin2.sin_family = AF_INET;
	sin2.sin_port = 0;
	sin2.sin_addr.s_addr = INADDR_ANY;
	sin3.sin_family = AF_INET;
	sin3.sin_port = 0;
	sin3.sin_addr.s_addr = htonl(0x0AFFFEFD);
	sun.sun_family = AF_UNIX;
	strncpy(sun.sun_path, ".", sizeof(sun.sun_path));
	inet_socket = socket(PF_INET, SOCK_STREAM, 0);
	dev_null = open("/dev/null", O_WRONLY);
}
void test_cleanup(void)
{
	close(inet_socket);
	close(dev_null);
}

