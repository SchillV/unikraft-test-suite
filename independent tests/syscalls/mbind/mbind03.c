#include "incl.h"
#ifdef HAVE_NUMA_H
# include <numa.h>
# include <numaif.h>
# include "mbind.h"
#endif
#ifdef HAVE_NUMA_V2

size_t page_size;

struct tst_nodemap *nodes;

void setup(void)
{
	page_size = getpagesize();
	nodes = tst_get_nodemap(TST_NUMA_MEM, 2 * page_size / 1024);
	if (nodes->cnt <= 1)
		tst_brk(TCONF, "Test requires at least two NUMA memory nodes");
}

void cleanup(void)
{
	tst_nodemap_free(nodes);
}

int  verify_policy(int mode, unsigned flag)
{
	struct bitmask *bm = numa_allocate_nodemask();
	unsigned int i;
	void *ptr;
	unsigned long size = page_size;
	unsigned int node = 0;
	ptr = tst_numa_map(NULL, size);
	tst_nodemap_reset_counters(nodes);
	tst_numa_fault(ptr, size);
	tst_nodemap_count_pages(nodes, ptr, size);
	tst_nodemap_print_counters(nodes);
	for (i = 0; i < nodes->cnt; i++) {
		if (!nodes->counters[i]) {
			node = nodes->map[i];
			tst_res(TINFO, "Attempting to move to node %i", node);
			numa_bitmask_setbit(bm, node);
			break;
		}
	}
mbind(ptr, size, mode, bm->maskp, bm->size + 1, flag);
	if (TST_RET) {
		tst_res(TFAIL | TTERRNO,
		        "mbind(%s, %s) node %u",
		        tst_mempolicy_mode_name(mode), mbind_flag_name(flag), node);
		goto exit;
	} else {
		tst_res(TPASS, "mbind(%s, %s) node %u succeded",
		        tst_mempolicy_mode_name(mode), mbind_flag_name(flag), node);
	}
	tst_nodemap_reset_counters(nodes);
	tst_nodemap_count_pages(nodes, ptr, size);
	for (i = 0; i < nodes->cnt; i++) {
		if (nodes->map[i] == node) {
			if (nodes->counters[i] == 1) {
				tst_res(TPASS, "Node %u allocated %u", node, 1);
			} else {
				tst_res(TFAIL, "Node %u allocated %u, expected %u",
				        node, nodes->counters[i], 0);
			}
			continue;
		}
		if (nodes->counters[i]) {
			tst_res(TFAIL, "Node %u allocated %u, expected 0",
			        i, nodes->counters[i]);
		}
	}
exit:
	tst_numa_unmap(ptr, size);
	numa_free_nodemask(bm);
}

const int modes[] = {
	MPOL_PREFERRED,
	MPOL_BIND,
	MPOL_INTERLEAVE,
};

int  verify_mbind(unsigned int n)
{
int 	verify_policy(modes[n], MPOL_MF_MOVE);
int 	verify_policy(modes[n], MPOL_MF_MOVE_ALL);
}

void main(){
	setup();
	cleanup();
}
