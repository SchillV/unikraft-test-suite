#include "incl.h"
#define SNDBUF	(0xffffff00)

int sockfd;

void run(void)
{
	unsigned int sndbuf, rec_sndbuf;
	socklen_t optlen;
	sndbuf = SNDBUF;
	rec_sndbuf = 0;
	optlen = sizeof(sndbuf);
	setsockopt(sockfd, SOL_SOCKET, SO_SNDBUFFORCE, &sndbuf, optlen);
	getsockopt(sockfd, SOL_SOCKET, SO_SNDBUF, &rec_sndbuf, &optlen);
	tst_res(TINFO, "Try to set send buffer size to: %u", sndbuf);
	tst_res(TINFO, "Send buffer size was set to: %d", rec_sndbuf);
	if ((int)rec_sndbuf < 0)
		tst_res(TFAIL, "Was able to set negative send buffer size!");
	else
		tst_res(TPASS, "Was unable to set negative send buffer size!");
}

void setup(void)
{
	sockfd = socket(AF_INET, SOCK_DGRAM, 0);
}

void cleanup(void)
{
	if (sockfd > 0)
		close(sockfd);
}

void main(){
	setup();
	cleanup();
}
