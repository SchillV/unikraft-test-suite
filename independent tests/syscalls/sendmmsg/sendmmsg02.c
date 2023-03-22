#include "incl.h"
#define _GNU_SOURCE
#define VLEN 1

int send_sockfd;

struct mmsghdr *snd_msg;

void *bad_addr;

int bad_fd = -1;
struct test_case {
	const char *desc;
	int *fd;
	int exp_errno;
	struct mmsghdr **msg_vec;
};

struct test_case tcase[] = {
	{
		.desc = "bad file descriptor",
		.fd = &bad_fd,
		.msg_vec = &snd_msg,
		.exp_errno = EBADF,
	},
	{
		.desc = "invalid msgvec address",
		.fd = &send_sockfd,
		.msg_vec = (void*)&bad_addr,
		.exp_errno = EFAULT,
	},
};

void do_test(unsigned int i)
{
	struct time64_variants *tv = &variants[tst_variant];
	struct test_case *tc = &tcase[i];
	TST_EXP_FAIL(tv->sendmmsg(*tc->fd, *tc->msg_vec, VLEN, 0),
	             tc->exp_errno, "sendmmsg() %s", tc->desc);
}

void setup(void)
{
	send_sockfd = socket(AF_INET, SOCK_DGRAM, 0);
	tst_res(TINFO, "Testing variant: %s", variants[tst_variant].desc);
}

void cleanup(void)
{
	if (send_sockfd > 0)
		close(send_sockfd);
}

void main(){
	setup();
	cleanup();
}
