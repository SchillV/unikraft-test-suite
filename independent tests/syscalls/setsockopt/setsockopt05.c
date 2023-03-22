#include "incl.h"
#define BUFSIZE 4000

struct sockaddr_in addr;

int dst_sock = -1;

void setup(void)
{
	struct ifreq ifr;
	socklen_t addrlen = sizeof(addr);
	tst_setup_netns();
	tst_init_sockaddr_inet_bin(&addr, INADDR_LOOPBACK, 0);
	dst_sock = socket(AF_INET, SOCK_DGRAM, 0);
	strcpy(ifr.ifr_name, "lo");
	ifr.ifr_mtu = 1500;
	ioctl(dst_sock, SIOCSIFMTU, &ifr);
	ifr.ifr_flags = IFF_UP;
	ioctl(dst_sock, SIOCSIFFLAGS, &ifr);
	bind(dst_sock, dst_sock, struct sockaddr *)&addr, addrlen);
	getsockname(dst_sock, dst_sock, struct sockaddr*)&addr, &addrlen);
}

void cleanup(void)
{
	if (dst_sock != -1)
		close(dst_sock);
}

void run(void)
{
	int sock, i;
	char buf[BUFSIZE];
	memset(buf, 0x42, BUFSIZE);
	for (i = 0; i < 1000; i++) {
		sock = socket(AF_INET, SOCK_DGRAM, 0);
		connect(sock, sock, struct sockaddr *)&addr, sizeofaddr));
		send(1, sock, buf, BUFSIZE, MSG_MORE);
		setsockopt_int(sock, SOL_SOCKET, SO_NO_CHECK, 1);
		send(sock, buf, 1, 0);
		close(sock);
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
