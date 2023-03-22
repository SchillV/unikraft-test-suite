#include "incl.h"

volatile int fd = -1;

struct sockaddr_ll addr1, addr2;

struct tst_fzsync_pair fzsync_pair;

void setup(void)
{
	struct ifreq ifr;
	tst_setup_netns();
	fd = socket(AF_PACKET, SOCK_DGRAM, PF_PACKET);
	strcpy(ifr.ifr_name, "lo");
	ioctl(fd, SIOCGIFINDEX, &ifr);
	close(fd);
	addr1.sll_family = AF_PACKET;
	addr1.sll_ifindex = ifr.ifr_ifindex;
	addr2.sll_family = AF_PACKET;
	fzsync_pair.exec_loops = 10000;
	tst_fzsync_pair_init(&fzsync_pair);
}

void cleanup(void)
{
	tst_fzsync_pair_cleanup(&fzsync_pair);
}

void do_bind(void)
{
	bind(fd, fd, struct sockaddr *)&addr1, sizeofaddr1));
	bind(fd, fd, struct sockaddr *)&addr2, sizeofaddr2));
}

void *thread_run(void *arg)
{
	while (tst_fzsync_run_b(&fzsync_pair)) {
		tst_fzsync_start_race_b(&fzsync_pair);
		do_bind();
		tst_fzsync_end_race_b(&fzsync_pair);
	}
	return arg;
}

void run(void)
{
	struct ifreq ifr;
	tst_fzsync_pair_reset(&fzsync_pair, thread_run);
	strcpy(ifr.ifr_name, "lo");
	while (tst_fzsync_run_a(&fzsync_pair)) {
		fd = socket(AF_PACKET, SOCK_DGRAM, PF_PACKET);
		ifr.ifr_flags = 0;
		ioctl(fd, SIOCSIFFLAGS, &ifr);
		ifr.ifr_flags = IFF_UP;
		tst_fzsync_start_race_a(&fzsync_pair);
		ioctl(fd, SIOCSIFFLAGS, &ifr);
		tst_fzsync_end_race_a(&fzsync_pair);
		close(fd);
	}
	tst_res(TPASS, "Nothing bad happened (yet)");
}

void main(){
	setup();
	cleanup();
}
