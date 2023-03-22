#include "incl.h"
#define IOVEC_COUNT 4
#define PACKET_SIZE 100

int sockfd = -1;

struct msghdr msg;

struct sockaddr_in addr;

struct iovec iov[IOVEC_COUNT];

unsigned char buf[PACKET_SIZE];

struct tst_fzsync_pair fzsync_pair;

void setup(void)
{
	int i;
	tst_setup_netns();
	sockfd = socket(AF_INET, SOCK_RAW, IPPROTO_ICMP);
	memset(buf, 0xcc, PACKET_SIZE);
	for (i = 0; i < IOVEC_COUNT; i++) {
		iov[i].iov_base = buf;
		iov[i].iov_len = PACKET_SIZE;
	}
	msg.msg_name = &addr;
	msg.msg_namelen = sizeof(addr);
	msg.msg_iov = iov;
	msg.msg_iovlen = IOVEC_COUNT;
	fzsync_pair.exec_loops = 100000;
	tst_fzsync_pair_init(&fzsync_pair);
}

void cleanup(void)
{
	if (sockfd > 0)
		close(sockfd);
	tst_fzsync_pair_cleanup(&fzsync_pair);
}

void *thread_run(void *arg)
{
	int val = 0;
	while (tst_fzsync_run_b(&fzsync_pair)) {
		tst_fzsync_start_race_b(&fzsync_pair);
		setsockopt(sockfd, SOL_IP, IP_HDRINCL, &val, sizeof(val));
		tst_fzsync_end_race_b(&fzsync_pair);
	}
	return arg;
}

void run(void)
{
	int hdrincl = 1;
	tst_fzsync_pair_reset(&fzsync_pair, thread_run);
	while (tst_fzsync_run_a(&fzsync_pair)) {
		setsockopt_int(sockfd, SOL_IP, IP_HDRINCL, hdrincl);
		tst_fzsync_start_race_a(&fzsync_pair);
		sendmsg(sockfd, &msg, 0);
		tst_fzsync_end_race_a(&fzsync_pair);
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
