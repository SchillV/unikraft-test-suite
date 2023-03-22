#include "incl.h"

int sock = -1;

unsigned int pagesize;

void setup(void)
{
	pagesize = sysconf(_SC_PAGESIZE);
	tst_setup_netns();
}

void run(void)
{
	unsigned int i, version = TPACKET_V3;
	struct tpacket_req3 req = {
		.tp_block_size = 4 * pagesize,
		.tp_frame_size = TPACKET_ALIGNMENT << 7,
		.tp_retire_blk_tov = 64,
		.tp_feature_req_word = TP_FT_REQ_FILL_RXHASH
	};
	for (i = 0; i < 5; i++) {
		req.tp_block_nr = 256;
		req.tp_frame_nr = req.tp_block_size * req.tp_block_nr;
		req.tp_frame_nr /= req.tp_frame_size;
		sock = socket(AF_PACKET, SOCK_RAW, 0);
setsockopt(sock, SOL_PACKET, PACKET_VERSION, &versio;
			sizeof(version)));
		if (TST_RET == -1 && TST_ERR == EINVAL)
			tst_brk(TCONF | TTERRNO, "TPACKET_V3 not supported");
		if (TST_RET) {
			tst_brk(TBROK | TTERRNO,
				"setsockopt(PACKET_VERSION, TPACKET_V3)");
		}
		setsockopt(sock, SOL_PACKET, PACKET_RX_RING, &req,
			sizeof(req));
		req.tp_block_nr = 0;
		req.tp_frame_nr = 0;
		setsockopt(sock, SOL_PACKET, PACKET_RX_RING, &req,
			sizeof(req));
		setsockopt_int(sock, SOL_PACKET, PACKET_VERSION,
			TPACKET_V2);
		setsockopt(sock, SOL_PACKET, PACKET_RX_RING, &req,
			sizeof(req));
		close(sock);
		usleep(req.tp_retire_blk_tov * 3000);
		if (tst_taint_check()) {
			tst_res(TFAIL, "Kernel is vulnerable");
			return;
		}
	}
	tst_res(TPASS, "Nothing bad happened, probably");
}

void cleanup(void)
{
	if (sock >= 0)
		close(sock);
}

void main(){
	setup();
	cleanup();
}
