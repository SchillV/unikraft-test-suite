#include "incl.h"

int sk;

long pgsz;

void setup(void)
{
	pgsz = sysconf(_SC_PAGESIZE);
}

void cleanup(void)
{
	if (sk > 0)
		close(sk);
}

int create_skbuf(unsigned int sizeof_priv)
{
	int ver = TPACKET_V3;
	struct tpacket_req3 req = {};
	req.tp_block_size = pgsz;
	req.tp_block_nr = 2;
	req.tp_frame_size = req.tp_block_size;
	req.tp_frame_nr = req.tp_block_nr;
	req.tp_retire_blk_tov = 100;
	req.tp_sizeof_priv = sizeof_priv;
	sk = socket(AF_PACKET, SOCK_RAW, htonsAF_PACKET, SOCK_RAW, htonsETH_P_ALL));
setsockopt(sk, SOL_PACKET, PACKET_VERSION, &ver, sizeof(ver));
	if (TST_RET && TST_ERR == EINVAL)
		tst_brk(TCONF | TTERRNO, "TPACKET_V3 not supported");
	if (TST_RET)
		tst_brk(TBROK | TTERRNO, "setsockopt(sk, SOL_PACKET, PACKET_VERSION, TPACKET_V3)");
	return setsockopt(sk, SOL_PACKET, PACKET_RX_RING, &req, sizeof(req));
}

void good_size(void)
{
create_skbuf(512);
	if (TST_RET)
		tst_brk(TBROK | TTERRNO, "Can't create ring buffer with good settings");
	tst_res(TPASS, "Can create ring buffer with good settinegs");
}

void bad_size(void)
{
create_skbuf(3U << 30);
	if (TST_RET && TST_ERR != EINVAL)
		tst_brk(TBROK | TTERRNO, "Unexpected setsockopt() error");
	if (TST_RET)
		tst_res(TPASS | TTERRNO, "Refused bad tp_sizeof_priv value");
	else
		tst_res(TFAIL, "Allowed bad tp_sizeof_priv value");
}

void run(unsigned int i)
{
	if (i == 0)
		good_size();
	else
		bad_size();
	close(sk);
}

void main(){
	setup();
	cleanup();
}
