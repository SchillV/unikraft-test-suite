#include "incl.h"
#define _GNU_SOURCE
#define BUFSIZE 16
#define VLEN 2

int send_sockfd;

int receive_sockfd;

struct mmsghdr *snd_msg, *rcv_msg;

struct iovec *snd1, *snd2, *rcv1, *rcv2;

void run(void)
{
	struct time64_variants *tv = &variants[tst_variant];
	struct tst_ts timeout;
	int retval;
	retval = tv->sendmmsg(send_sockfd, snd_msg, VLEN, 0);
	if (retval < 0 || snd_msg[0].msg_len != 6 || snd_msg[1].msg_len != 6) {
		tst_res(TFAIL | TERRNO, "sendmmsg() failed");
		return;
	}
	memset(rcv1->iov_base, 0, rcv1->iov_len);
	memset(rcv2->iov_base, 0, rcv2->iov_len);
	timeout.type = tv->ts_type;
	tst_ts_set_sec(&timeout, 1);
	tst_ts_set_nsec(&timeout, 0);
	retval = tv->recvmmsg(receive_sockfd, rcv_msg, VLEN, 0, tst_ts_get(&timeout));
	if (retval == -1) {
		tst_res(TFAIL | TERRNO, "recvmmsg() failed");
		return;
	}
	if (retval != 2) {
		tst_res(TFAIL, "Received unexpected number of messages (%d)",
			retval);
		return;
	}
	if (memcmp(rcv1->iov_base, "onetwo", 6))
		tst_res(TFAIL, "Error in first received message");
	else
		tst_res(TPASS, "First message received successfully");
	if (memcmp(rcv2->iov_base, "three", 5))
		tst_res(TFAIL, "Error in second received message");
	else
		tst_res(TPASS, "Second message received successfully");
}

void setup(void)
{
	struct sockaddr_in addr;
	unsigned int port = TST_GET_UNUSED_PORT(AF_INET, SOCK_DGRAM);
	send_sockfd = socket(AF_INET, SOCK_DGRAM, 0);
	receive_sockfd = socket(AF_INET, SOCK_DGRAM, 0);
	addr.sin_family = AF_INET;
	addr.sin_addr.s_addr = htonl(INADDR_LOOPBACK);
	addr.sin_port = port;
	bind(receive_sockfd, receive_sockfd, struct sockaddr *)&addr, sizeofaddr));
	connect(send_sockfd, send_sockfd, struct sockaddr *)&addr, sizeofaddr));
	memcpy(snd1[0].iov_base, "one", snd1[0].iov_len);
	memcpy(snd1[1].iov_base, "two", snd1[1].iov_len);
	memcpy(snd2->iov_base, "three3", snd2->iov_len);
	memset(snd_msg, 0, VLEN * sizeof(*snd_msg));
	snd_msg[0].msg_hdr.msg_iov = snd1;
	snd_msg[0].msg_hdr.msg_iovlen = 2;
	snd_msg[1].msg_hdr.msg_iov = snd2;
	snd_msg[1].msg_hdr.msg_iovlen = 1;
	memset(rcv_msg, 0, VLEN * sizeof(*rcv_msg));
	rcv_msg[0].msg_hdr.msg_iov = rcv1;
	rcv_msg[0].msg_hdr.msg_iovlen = 1;
	rcv_msg[1].msg_hdr.msg_iov = rcv2;
	rcv_msg[1].msg_hdr.msg_iovlen = 1;
	tst_res(TINFO, "Testing variant: %s", variants[tst_variant].desc);
}

void cleanup(void)
{
	if (send_sockfd > 0)
		close(send_sockfd);
	if (receive_sockfd > 0)
		close(receive_sockfd);
}

void main(){
	setup();
	cleanup();
}
