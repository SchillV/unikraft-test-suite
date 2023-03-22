#include "incl.h"
#define _GNU_SOURCE
#define SENDSIZE 16
#define RECVSIZE 32

int sock = -1, dst_sock = -1, listen_sock = -1;

struct sockaddr_in addr;

char sendbuf[SENDSIZE];

void do_send(int sock, void *buf, size_t size, int flags)
{
	send(1, sock, buf, size, flags);
}

void do_sendto(int sock, void *buf, size_t size, int flags)
{
	sendto(1, sock, buf, size, flags, 1, sock, buf, size, flags, struct sockaddr *)&addr,
		sizeof(addr));
}

void do_sendmsg(int sock, void *buf, size_t size, int flags)
{
	struct msghdr msg;
	struct iovec iov;
	iov.iov_base = buf;
	iov.iov_len = size;
	msg.msg_name = &addr;
	msg.msg_namelen = sizeof(addr);
	msg.msg_iov = &iov;
	msg.msg_iovlen = 1;
	msg.msg_control = NULL;
	msg.msg_controllen = 0;
	msg.msg_flags = 0;
	sendmsg(size, sock, &msg, flags);
}

struct test_case {
	int domain, type, protocol;
	void (*send)(int sock, void *buf, size_t size, int flags);
	int needs_connect, needs_accept;
	const char *name;
} testcase_list[] = {
	{AF_INET, SOCK_STREAM, 0, do_send, 1, 1, "TCP send"},
	{AF_INET, SOCK_DGRAM, 0, do_send, 1, 0, "UDP send"},
	{AF_INET, SOCK_DGRAM, 0, do_sendto, 0, 0, "UDP sendto"},
	{AF_INET, SOCK_DGRAM, 0, do_sendmsg, 0, 0, "UDP sendmsg"}
};

void setup(void)
{
	memset(sendbuf, 0x42, SENDSIZE);
}

int check_recv(int sock, long expsize, int loop)
{
	char recvbuf[RECVSIZE] = {0};
	while (1) {
recv(sock, recvbuf, RECVSIZE, MSG_DONTWAIT);
		if (TST_RET == -1) {
			if (TST_ERR == EAGAIN || TST_ERR == EWOULDBLOCK) {
				if (expsize)
					continue;
				else
					break;
			}
			tst_res(TFAIL | TTERRNO, "recv() error at step %d, expsize %ld",
				loop, expsize);
			return 0;
		}
		if (TST_RET < 0) {
			tst_res(TFAIL | TTERRNO, "recv() returns %ld at step %d, expsize %ld",
				TST_RET, loop, expsize);
			return 0;
		}
		if (TST_RET != expsize) {
			tst_res(TFAIL, "recv() read %ld bytes, expected %ld, step %d",
				TST_RET, expsize, loop);
			return 0;
		}
		return 1;
	}
	return 1;
}

void cleanup(void)
{
	if (sock >= 0)
		close(sock);
	if (dst_sock >= 0 && dst_sock != listen_sock)
		close(dst_sock);
	if (listen_sock >= 0)
		close(listen_sock);
}

void run(unsigned int n)
{
	int i, ret;
	struct test_case *tc = testcase_list + n;
	socklen_t len = sizeof(addr);
	tst_res(TINFO, "Tesing %s", tc->name);
	tst_init_sockaddr_inet_bin(&addr, INADDR_LOOPBACK, 0);
	listen_sock = socket(tc->domain, tc->type, tc->protocol);
	dst_sock = listen_sock;
	bind(listen_sock, listen_sock, struct sockaddr *)&addr, sizeofaddr));
	getsockname(listen_sock, listen_sock, struct sockaddr *)&addr, &len);
	if (tc->needs_accept)
		listen(listen_sock, 1);
	for (i = 0; i < 1000; i++) {
		sock = socket(tc->domain, tc->type, tc->protocol);
		if (tc->needs_connect)
			connect(sock, sock, struct sockaddr *)&addr, len);
		if (tc->needs_accept)
			dst_sock = accept(listen_sock, NULL, NULL);
		tc->send(sock, sendbuf, SENDSIZE, 0);
		ret = check_recv(dst_sock, SENDSIZE, i + 1);
		if (!ret)
			break;
		tc->send(sock, sendbuf, SENDSIZE, MSG_MORE);
		ret = check_recv(dst_sock, 0, i + 1);
		if (!ret)
			break;
		tc->send(sock, sendbuf, 1, 0);
		ret = check_recv(dst_sock, SENDSIZE + 1, i + 1);
		if (!ret)
			break;
		close(sock);
		if (dst_sock != listen_sock)
			close(dst_sock);
	}
	if (ret)
		tst_res(TPASS, "MSG_MORE works correctly");
	cleanup();
	dst_sock = -1;
}

