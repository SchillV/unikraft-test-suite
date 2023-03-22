#include "incl.h"

int sock = -1;

unsigned int pagesize;

struct tst_fzsync_pair fzsync_pair;

void setup(void)
{
	pagesize = sysconf(_SC_PAGESIZE);
	tst_setup_netns();
	 * Reproducing the bug on unpatched system takes <15 loops. The test
	 * is slow and the bug is mostly harmless so don't waste too much
	 * time.
	 */
	fzsync_pair.exec_loops = 500;
	tst_fzsync_pair_init(&fzsync_pair);
}

void *thread_run(void *arg)
{
	unsigned int val = 1 << 30;
	while (tst_fzsync_run_b(&fzsync_pair)) {
		tst_fzsync_start_race_b(&fzsync_pair);
		setsockopt(sock, SOL_PACKET, PACKET_RESERVE, &val, sizeof(val));
		tst_fzsync_end_race_b(&fzsync_pair);
	}
	return arg;
}

void run(void)
{
	unsigned int val, version = TPACKET_V3;
	socklen_t vsize = sizeof(val);
	struct tpacket_req3 req = {
		.tp_block_size = pagesize,
		.tp_block_nr = 1,
		.tp_frame_size = pagesize,
		.tp_frame_nr = 1,
		.tp_retire_blk_tov = 100
	};
	tst_fzsync_pair_reset(&fzsync_pair, thread_run);
	while (tst_fzsync_run_a(&fzsync_pair)) {
		sock = socket(AF_PACKET, SOCK_RAW, htonsAF_PACKET, SOCK_RAW, htonsETH_P_ALL));
setsockopt(sock, SOL_PACKET, PACKET_VERSION, &versio;
			sizeof(version)));
		if (TST_RET == -1 && TST_ERR == EINVAL)
			tst_brk(TCONF | TTERRNO, "TPACKET_V3 not supported");
		if (TST_RET) {
			tst_brk(TBROK | TTERRNO,
				"setsockopt(PACKET_VERSION, TPACKET_V3");
		}
		tst_fzsync_start_race_a(&fzsync_pair);
setsockopt(sock, SOL_PACKET, PACKET_RX_RING, &re;
			sizeof(req)));
		tst_fzsync_end_race_a(&fzsync_pair);
		getsockopt(sock, SOL_PACKET, PACKET_RESERVE, &val, &vsize);
		close(sock);
		if (TST_RET == -1 && TST_ERR == EINVAL) {
			tst_fzsync_pair_add_bias(&fzsync_pair, 1);
			continue;
		}
		if (TST_RET) {
			tst_brk(TBROK | TTERRNO,
				"Invalid setsockopt() return value");
		}
		if (val > req.tp_block_size) {
			tst_res(TFAIL, "PACKET_RESERVE checks bypassed");
			return;
		}
	}
	tst_res(TPASS, "Cannot reproduce bug");
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
