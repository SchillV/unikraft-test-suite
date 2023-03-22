#include "incl.h"
#ifndef AF_RDS
# define AF_RDS 21
#endif

void setup(void)
{
	int res;
	res = socket(AF_RDS, SOCK_SEQPACKET, 0);
	if (res == -1) {
		if (errno == EAFNOSUPPORT)
			tst_brk(TCONF, "rds was not supported");
		else
			tst_brk(TBROK | TERRNO, "socket() failed with rds");
	}
	close(res);
}

void client(void)
{
	int sock_fd1;
	char send_buf[128] = "hello world";
	struct sockaddr_in server_addr;
	struct sockaddr_in to_addr;
	struct msghdr msg;
	struct iovec iov;
	sock_fd1 = socket(AF_RDS, SOCK_SEQPACKET, 0);
	memset(&server_addr, 0, sizeof(server_addr));
	server_addr.sin_family = AF_INET;
	server_addr.sin_addr.s_addr = inet_addr("127.0.0.1");
	server_addr.sin_port = htons(4001);
	bind(sock_fd1, sock_fd1, struct sockaddr *) &server_addr, sizeofserver_addr));
	memset(&to_addr, 0, sizeof(to_addr));
	to_addr.sin_family = AF_INET;
	to_addr.sin_addr.s_addr = inet_addr("127.0.0.1");
	to_addr.sin_port = htons(4000);
	msg.msg_name = &to_addr;
	msg.msg_namelen = sizeof(to_addr);
	msg.msg_iov = &iov;
	msg.msg_iovlen = 1;
	msg.msg_iov->iov_base = send_buf;
	msg.msg_iov->iov_len = strlen(send_buf) + 1;
	msg.msg_control = 0;
	msg.msg_controllen = 0;
	msg.msg_flags = 0;
	if (sendmsg(sock_fd1, &msg, 0) == -1) {
		tst_brk(TBROK | TERRNO,
			"sendmsg() failed to send data to server");
	}
	TST_CHECKPOINT_WAIT(1);
	close(sock_fd1);
}

void server(void)
{
	int sock_fd2;
	

char recv_buf[128];
	struct sockaddr_in server_addr;
	struct sockaddr_in from_addr;
	struct msghdr msg;
	struct iovec iov;
	sock_fd2 = socket(AF_RDS, SOCK_SEQPACKET, 0);
	memset(&server_addr, 0, sizeof(server_addr));
	server_addr.sin_family = AF_INET;
	server_addr.sin_addr.s_addr = inet_addr("127.0.0.1");
	server_addr.sin_port = htons(4000);
	bind(sock_fd2, sock_fd2, struct sockaddr *) &server_addr, sizeofserver_addr));
	msg.msg_name = &from_addr;
	msg.msg_namelen = sizeof(from_addr) + 16;
	msg.msg_iov = &iov;
	msg.msg_iovlen = 1;
	msg.msg_iov->iov_base = recv_buf;
	msg.msg_iov->iov_len = 128;
	msg.msg_control = 0;
	msg.msg_controllen = 0;
	msg.msg_flags = 0;
	TST_CHECKPOINT_WAKE(0);
	recvmsg(0, sock_fd2, &msg, 0);
	if (msg.msg_namelen != sizeof(from_addr)) {
		tst_res(TFAIL, "msg_namelen was set to %u incorrectly, "
			"expected %lu", msg.msg_namelen, sizeof(from_addr));
	} else {
		tst_res(TPASS, "msg_namelen was set to %u correctly",
			msg.msg_namelen);
	}
	TST_CHECKPOINT_WAKE(1);
	close(sock_fd2);
}

int  verify_recvmsg(
{
	pid_t pid;
	pid = fork();
	if (pid == 0) {
		TST_CHECKPOINT_WAIT(0);
		client();
	} else {
		server();
		tst_reap_children();
	}
}

void main(){
	setup();
	cleanup();
}
