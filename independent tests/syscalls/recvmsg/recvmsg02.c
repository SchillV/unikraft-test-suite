#include "incl.h"

const char msg[] = "Michael Gilfix was here\341\210\264\r\n";

const unsigned msglen = ARRAY_SIZE(msg) - 1;

unsigned char buff[25];

const int bufflen = ARRAY_SIZE(buff);

int sdr, sdw;

int  verify_recvmsg(
{
	struct sockaddr_in6 addr_init = {
		.sin6_family	= AF_INET6,
		.sin6_port	= htons(0),
		.sin6_addr	= IN6ADDR_LOOPBACK_INIT,
	};
	struct sockaddr_in6 addr_r, addr_w, addr_f;
	socklen_t addrlen_r, addrlen_w;
	struct iovec iov = {
		.iov_base	= buff,
		.iov_len	= sizeof(buff),
	};
	struct msghdr msghdr = {
		.msg_name	= &addr_f,
		.msg_namelen	= sizeof(addr_f),
		.msg_iov	= &iov,
		.msg_iovlen	= 1,
		.msg_control	= NULL,
		.msg_controllen	= 0,
		.msg_flags	= 0,
	};
	int R;
	sdr = socket(PF_INET6, SOCK_DGRAM | SOCK_CLOEXEC, IPPROTO_IP);
	bind(sdr, sdr, struct sockaddr*)&addr_init, sizeofaddr_init));
	addrlen_r = sizeof(addr_r);
	getsockname(sdr, sdr, struct sockaddr*)&addr_r, &addrlen_r);
	sdw = socket(PF_INET6, SOCK_DGRAM|SOCK_CLOEXEC, IPPROTO_IP);
	bind(sdw, sdw, struct sockaddr*)&addr_init, sizeofaddr_init));
	addrlen_w = sizeof(addr_w);
	getsockname(sdw, sdw, struct sockaddr*)&addr_w, &addrlen_w);
	R = sendto(sdw, msg, msglen, 0, (struct sockaddr*)&addr_r, addrlen_r);
	if (R < 0)
		tst_brk(TBROK | TERRNO, "sendto()");
	R = recvmsg(sdr, &msghdr, MSG_PEEK);
	if (R < 0) {
		tst_res(TFAIL | TERRNO, "recvmsg(..., MSG_PEEK)");
		return;
	}
	tst_res(TINFO, "received %d bytes", R);
	if ((R == bufflen) && !memcmp(msg, buff, R))
		tst_res(TPASS, "recvmsg(..., MSG_PEEK) works fine");
	else
		tst_res(TPASS, "recvmsg(..., MSG_PEEK) failed");
	close(sdw);
	close(sdr);
}

void cleanup(void)
{
	if (sdw > 0)
		close(sdw);
	if (sdr > 0)
		close(sdr);
}

void main(){
	setup();
	cleanup();
}
