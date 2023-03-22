#include "incl.h"
#define _GNU_SOURCE

int send_sockfd;

int receive_sockfd;
#define VLEN 1

struct mmsghdr *msg;

struct iovec *iov;

void *bad_addr;

int bad_fd = -1;

struct tst_ts ts;
struct test_case {
	const char *desc;
	int *fd;
	long tv_sec;
	long tv_nsec;
	int exp_errno;
	struct mmsghdr **msg_vec;
	int bad_ts_addr;
};

struct test_case tcase[] = {
	{
		.desc = "bad socket file descriptor",
		.fd = &bad_fd,
		.exp_errno = EBADF,
		.msg_vec = &msg,
	},
	{
		.desc = "bad message vector address",
		.fd = &receive_sockfd,
		.exp_errno = EFAULT,
		.msg_vec = (void*)&bad_addr,
	},
	{
		.desc = "negative seconds in timeout",
		.fd = &receive_sockfd,
		.tv_sec = -1,
		.tv_nsec = 0,
		.exp_errno = EINVAL,
		.msg_vec = &msg,
	},
	{
		.desc = "overflow in nanoseconds in timeout",
		.fd = &receive_sockfd,
		.tv_sec = 1,
		.tv_nsec = 1000000001,
		.exp_errno = EINVAL,
		.msg_vec = &msg,
	},
	{
		.desc = "bad timeout address",
		.fd = &receive_sockfd,
		.exp_errno = EFAULT,
		.msg_vec = &msg,
		.bad_ts_addr = 1,
	}
};

void do_test(unsigned int i)
{
	struct time64_variants *tv = &variants[tst_variant];
	struct test_case *tc = &tcase[i];
	void *timeout;
	ts.type = tv->ts_type;
	tst_ts_set_sec(&ts, tc->tv_sec);
	tst_ts_set_nsec(&ts, tc->tv_nsec);
	if (tc->bad_ts_addr)
		timeout = bad_addr;
	else
		timeout = tst_ts_get(&ts);
	TST_EXP_FAIL2(tv->recvmmsg(*tc->fd, *tc->msg_vec, VLEN, 0, timeout),
	             tc->exp_errno, "recvmmsg() %s", tc->desc);
}

void setup(void)
{
	struct sockaddr_in addr;
	unsigned int port = TST_GET_UNUSED_PORT(AF_INET, SOCK_DGRAM);
	struct time64_variants *tv = &variants[tst_variant];
	tst_res(TINFO, "Testing variant: %s", variants[tst_variant].desc);
	send_sockfd = socket(AF_INET, SOCK_DGRAM, 0);
	receive_sockfd = socket(AF_INET, SOCK_DGRAM, 0);
	addr.sin_family = AF_INET;
	addr.sin_addr.s_addr = htonl(INADDR_LOOPBACK);
	addr.sin_port = port;
	bind(receive_sockfd, receive_sockfd, struct sockaddr *)&addr, sizeofaddr));
	connect(send_sockfd, send_sockfd, struct sockaddr *)&addr, sizeofaddr));
	msg[0].msg_hdr.msg_iov = iov;
	msg[0].msg_hdr.msg_iovlen = 1;
tv->sendmmsg(send_sockfd, msg, 1, 0);
	if (TST_RET != 1) {
		tst_res(TFAIL | TTERRNO, "sendmmsg() failed");
		return;
	}
	bad_addr = tst_get_bad_addr(NULL);
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
