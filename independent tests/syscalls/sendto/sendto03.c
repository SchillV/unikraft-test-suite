#include "incl.h"
#define BUFSIZE 1024

int dst_sock = -1, sock = -1;

unsigned char buf[BUFSIZE];

struct sockaddr_ll bind_addr, addr;

void setup(void)
{
	struct ifreq ifr;
	tst_setup_netns();
	sock = socket(AF_PACKET, SOCK_RAW, htonsAF_PACKET, SOCK_RAW, htonsETH_P_ALL));
	strcpy(ifr.ifr_name, "lo");
	ifr.ifr_flags = IFF_UP;
	ioctl(sock, SIOCSIFFLAGS, &ifr);
	ioctl(sock, SIOCGIFINDEX, &ifr);
	close(sock);
	memset(buf, 0x42, BUFSIZE);
	bind_addr.sll_family = AF_PACKET;
	bind_addr.sll_protocol = htons(ETH_P_ALL);
	bind_addr.sll_ifindex = ifr.ifr_ifindex;
	addr.sll_family = AF_PACKET;
	addr.sll_ifindex = ifr.ifr_ifindex;
	addr.sll_halen = ETH_ALEN;
}

int check_tiny_frame(void)
{
	unsigned int val = (UINT_MAX - TPACKET2_HDRLEN) + 1;
	struct tpacket_req tpreq;
	tpreq.tp_block_size = sysconf(_SC_PAGESIZE);
	tpreq.tp_frame_size = TPACKET_ALIGNMENT;
	tpreq.tp_block_nr = 1;
	tpreq.tp_frame_nr = (tpreq.tp_block_size * tpreq.tp_block_nr) /
		tpreq.tp_frame_size;
	dst_sock = socket(AF_PACKET, SOCK_RAW, htonsAF_PACKET, SOCK_RAW, htonsETH_P_ALL));
	setsockopt_int(dst_sock, SOL_PACKET, PACKET_VERSION, TPACKET_V2);
setsockopt(dst_sock, SOL_PACKET, PACKET_RESERVE, &va;
		sizeof(val)));
	if (TST_RET == -1 && TST_ERR == EINVAL) {
		close(dst_sock);
		tst_res(TPASS | TTERRNO,
			"setsockopt(PACKET_RESERVE) value is capped");
		return 0;
	}
	if (TST_RET == -1) {
		tst_brk(TBROK | TTERRNO,
			"setsockopt(PACKET_RESERVE): unexpected error");
	}
	if (TST_RET) {
		tst_brk(TBROK | TTERRNO,
			"Invalid setsockopt(PACKET_RESERVE) return value");
	}
	tst_res(TINFO, "setsockopt(PACKET_RESERVE) accepted too large value");
	tst_res(TINFO, "Checking whether this will cause integer overflow...");
setsockopt(dst_sock, SOL_PACKET, PACKET_RX_RING, &tpre;
		sizeof(tpreq)));
	close(dst_sock);
	if (!TST_RET) {
		tst_res(TFAIL, "setsockopt(PACKET_RX_RING) accepted frame "
			"size smaller than packet header");
		return 0;
	}
	if (TST_RET != -1) {
		tst_brk(TBROK | TTERRNO,
			"Invalid setsockopt(PACKET_RX_RING) return value");
	}
	if (TST_ERR != EINVAL) {
		tst_brk(TBROK | TTERRNO,
			"setsockopt(PACKET_RX_RING): unexpeced error");
	}
	tst_res(TPASS | TTERRNO, "setsockopt(PACKET_RX_RING) frame size check "
		"rejects values smaller than packet header");
	return 0;
}

int check_vnet_hdr(void)
{
	struct tpacket_req tpreq;
	size_t blocksize = 0x800000, pagesize = sysconf(_SC_PAGESIZE);
	if (blocksize % pagesize)
		blocksize += pagesize - (blocksize % pagesize);
	tpreq.tp_block_size = blocksize;
	tpreq.tp_frame_size = 0x11000;
	tpreq.tp_block_nr = 1;
	tpreq.tp_frame_nr = (tpreq.tp_block_size * tpreq.tp_block_nr) /
		tpreq.tp_frame_size;
	dst_sock = socket(AF_PACKET, SOCK_RAW, htonsAF_PACKET, SOCK_RAW, htonsETH_P_ALL));
	setsockopt_int(dst_sock, SOL_PACKET, PACKET_VERSION, TPACKET_V2);
	setsockopt_int(dst_sock, SOL_PACKET, PACKET_VNET_HDR, 1);
	setsockopt_int(dst_sock, SOL_PACKET, PACKET_RESERVE, 0xffff - 75);
setsockopt(dst_sock, SOL_PACKET, PACKET_RX_RING, &tpre;
		sizeof(tpreq)));
	if (TST_RET == -1 && TST_ERR == EINVAL) {
		close(dst_sock);
		tst_res(TCONF, "PACKET_VNET_HDR and PACKET_RX_RING not "
			"supported together");
		return 0;
	}
	if (TST_RET == -1) {
		tst_brk(TBROK | TTERRNO,
			"setsockopt(PACKET_RX_RING): unexpected error");
	}
	if (TST_RET) {
		tst_brk(TBROK | TTERRNO,
			"Invalid setsockopt(PACKET_RX_RING) return value");
	}
	bind(dst_sock, dst_sock, struct sockaddr *)&bind_addr, sizeofaddr));
	sock = socket(AF_PACKET, SOCK_RAW, IPPROTO_RAW);
	sendto(1, sock, buf, BUFSIZE, 0, 1, sock, buf, BUFSIZE, 0, struct sockaddr *)&addr,
		sizeof(addr));
	close(sock);
	close(dst_sock);
}

int (*testcase_list[])(void) = {check_tiny_frame, check_vnet_hdr};

void run(unsigned int n)
{
	if (!testcase_list[n]())
		return;
	if (tst_taint_check()) {
		tst_res(TFAIL, "Kernel is vulnerable");
		return;
	}
	tst_res(TPASS, "Nothing bad happened, probably");
}

void cleanup(void)
{
	if (sock != -1)
		close(sock);
	if (dst_sock != -1)
		close(dst_sock);
}

void main(){
	setup();
	cleanup();
}
