#include "incl.h"

int listenfd = -1, fd = -1, confd1 = -1, confd2 = -1, confd3 = -1;

struct sockaddr_in6 bind_addr;

struct sockaddr_in bind_addr4, client_addr;

struct sockaddr reset_addr;

void setup(void)
{
	socklen_t size = sizeof(bind_addr);
	tst_init_sockaddr_inet6_bin(&bind_addr, &in6addr_any, 0);
	tst_init_sockaddr_inet_bin(&bind_addr4, INADDR_ANY, 0);
	memset(&reset_addr, 0, sizeof(reset_addr));
	reset_addr.sa_family = AF_UNSPEC;
	listenfd = socket(AF_INET6, SOCK_STREAM, IPPROTO_TCP);
	bind(listenfd, listenfd, struct sockaddr *)&bind_addr, sizeofbind_addr));
	listen(listenfd, 5);
	getsockname(listenfd, listenfd, struct sockaddr *)&bind_addr, &size);
	tst_init_sockaddr_inet(&client_addr, "127.0.0.1",
		htons(bind_addr.sin6_port));
}

void cleanup(void)
{
	if (confd3 >= 0)
		close(confd3);
	if (confd2 >= 0)
		close(confd2);
	if (confd1 >= 0)
		close(confd1);
	if (fd >= 0)
		close(fd);
	if (listenfd >= 0)
		close(listenfd);
}

void run(void)
{
	int i, addrlen, optval = AF_INET;
	struct sockaddr_storage client_addr2;
	for (i = 0; i < 1000; i++) {
		confd1 = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
		connect(confd1, confd1, struct sockaddr *)&client_addr,
			sizeof(client_addr));
		fd = accept(listenfd, NULL, NULL);
setsockopt(fd, SOL_IPV6, IPV6_ADDRFORM, &optva;
			sizeof(optval)));
		if (TST_RET == -1) {
			tst_res(TFAIL | TTERRNO,
				"setsockopt(IPV6_ADDRFORM) failed");
			return;
		}
		if (TST_RET != 0)
			tst_brk(TBROK | TTERRNO, "setsockopt(IPV6_ADDRFORM) "
				"returned invalid value");
		connect(fd, fd, struct sockaddr *)&reset_addr,
			sizeof(reset_addr));
		bind(fd, fd, struct sockaddr *)&bind_addr4,
			sizeof(bind_addr4));
		listen(fd, 5);
		addrlen = tst_get_connect_address(fd, &client_addr2);
		confd2 = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
		connect(confd2, confd2, struct sockaddr *)&client_addr2, addrlen);
		confd3 = accept(fd, NULL, NULL);
		close(confd3);
		close(confd2);
		close(confd1);
		close(fd);
		if (tst_taint_check()) {
			tst_res(TFAIL, "Kernel is vulnerable");
			return;
		}
	}
	tst_res(TPASS, "Nothing bad happened, probably");
}

void main(){
	setup();
	cleanup();
}
