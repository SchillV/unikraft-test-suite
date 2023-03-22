#include "incl.h"
#ifndef IPPROTO_SCTP
# define IPPROTO_SCTP	132
#endif

int sockfd;

struct sockaddr_in sa;

void setup(void)
{
	sockfd = socket(AF_INET, SOCK_STREAM, IPPROTO_SCTP);
	if (sockfd == -1) {
		if (errno == EPROTONOSUPPORT)
			tst_brk(TCONF, "sctp protocol was not supported");
		else
			tst_brk(TBROK | TERRNO, "socket() failed with sctp");
	}
	memset(&sa, 0, sizeof(sa));
	sa.sin_family = AF_INET;
	sa.sin_addr.s_addr = inet_addr("127.0.0.1");
	sa.sin_port = htons(11111);
}

void cleanup(void)
{
	if (sockfd > 0)
		close(sockfd);
}

int  verify_sendto(
{
sendto(sockfd, NULL, 1, 0, (struct sockaddr *) &sa, sizeof(sa));
	if (TST_RET != -1) {
		tst_res(TFAIL, "sendto(fd, NULL, ...) succeeded unexpectedly");
		return;
	}
	if (TST_ERR == EFAULT) {
		tst_res(TPASS | TTERRNO,
			"sendto(fd, NULL, ...) failed expectedly");
		return;
	}
	tst_res(TFAIL | TTERRNO,
		"sendto(fd, NULL, ...) failed unexpectedly, expected EFAULT");
}

void main(){
	setup();
	cleanup();
}
