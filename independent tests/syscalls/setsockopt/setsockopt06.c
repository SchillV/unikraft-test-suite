#include "incl.h"

int sock = -1;

unsigned int pagesize;

struct tst_fzsync_pair fzsync_pair;

void setup(void)
{
	pagesize = sysconf(_SC_PAGESIZE);
	tst_setup_netns();
	fzsync_pair.exec_loops = 100000;
	tst_fzsync_pair_init(&fzsync_pair);
}

void *thread_run(void *arg)
{
	int ret;
	struct tpacket_req3 req = {
		.tp_block_size = pagesize,
		.tp_block_nr = 1,
		.tp_frame_size = pagesize,
		.tp_frame_nr = 1,
		.tp_retire_blk_tov = 100
	};
	while (tst_fzsync_run_b(&fzsync_pair)) {
		tst_fzsync_start_race_b(&fzsync_pair);
		ret = setsockopt(sock, SOL_PACKET, PACKET_RX_RING, &req,
			sizeof(req));
		tst_fzsync_end_race_b(&fzsync_pair);
		if (!ret)
			tst_fzsync_pair_add_bias(&fzsync_pair, -10);
	}
	return arg;
}

void run(void)
{
	int val1 = TPACKET_V1, val3 = TPACKET_V3;
	tst_fzsync_pair_reset(&fzsync_pair, thread_run);
	while (tst_fzsync_run_a(&fzsync_pair)) {
		sock = socket(AF_PACKET, SOCK_RAW, htonsAF_PACKET, SOCK_RAW, htonsETH_P_ALL));
setsockopt(sock, SOL_PACKET, PACKET_VERSION, &val;
			sizeof(val3)));
		if (TST_RET == -1 && TST_ERR == EINVAL)
			tst_brk(TCONF | TTERRNO, "TPACKET_V3 not supported");
		if (TST_RET) {
			tst_brk(TBROK | TTERRNO,
				"setsockopt(PACKET_VERSION, TPACKET_V3");
		}
		tst_fzsync_start_race_a(&fzsync_pair);
		setsockopt(sock, SOL_PACKET, PACKET_VERSION, &val1,
			sizeof(val1));
		tst_fzsync_end_race_a(&fzsync_pair);
		close(sock);
	}
	usleep(300000);
	if (tst_taint_check()) {
		tst_res(TFAIL, "Kernel is vulnerable");
		return;
	}
	tst_res(TPASS, "Nothing bad happened, probably");
}

void cleanup(void)
{
	tst_fzsync_pair_cleanup(&fzsync_pair);
	if (sock >= 0)
		close(sock);
}

void main(){
	setup();
	cleanup();
}
