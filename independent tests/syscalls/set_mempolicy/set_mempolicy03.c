#include "incl.h"
#ifdef HAVE_NUMA_V2
# include <numaif.h>
# include <numa.h>
#endif
#define MNTPOINT "mntpoint"
#define PAGES_ALLOCATED 16u
#ifdef HAVE_NUMA_V2

size_t page_size;

struct tst_nodemap *nodes;

void setup(void)
{
	page_size = getpagesize();
	nodes = tst_get_nodemap(TST_NUMA_MEM, 2 * PAGES_ALLOCATED * page_size / 1024);
	if (nodes->cnt <= 1)
		tst_brk(TCONF, "Test requires at least two NUMA memory nodes");
}

void cleanup(void)
{
	tst_nodemap_free(nodes);
}

int  verify_mempolicy(unsigned int node, int mode)
{
	struct bitmask *bm = numa_allocate_nodemask();
	unsigned int i;
	numa_bitmask_setbit(bm, node);
set_mempolicy(mode, bm->maskp, bm->size+1);
	if (TST_RET) {
		tst_res(TFAIL | TTERRNO,
		        "set_mempolicy(%s) node %u",
		        tst_mempolicy_mode_name(mode), node);
		return;
	}
	tst_res(TPASS, "set_mempolicy(%s) node %u",
	        tst_mempolicy_mode_name(mode), node);
	numa_free_nodemask(bm);
	tst_nodemap_reset_counters(nodes);
	alloc_fault_count(nodes, MNTPOINT "/numa-test-file", PAGES_ALLOCATED * page_size);
	for (i = 0; i < nodes->cnt; i++) {
		if (nodes->map[i] == node) {
			if (nodes->counters[i] == PAGES_ALLOCATED) {
				tst_res(TPASS, "Node %u allocated %u",
				        node, PAGES_ALLOCATED);
			} else {
				tst_res(TFAIL, "Node %u allocated %u, expected %u",
				        node, nodes->counters[i], PAGES_ALLOCATED);
			}
			continue;
		}
		if (nodes->counters[i]) {
			tst_res(TFAIL, "Node %u allocated %u, expected 0",
			        node, nodes->counters[i]);
		}
	}
}

int  verify_set_mempolicy(unsigned int n)
{
	unsigned int i;
	int mode = n ? MPOL_PREFERRED : MPOL_BIND;
	for (i = 0; i < nodes->cnt; i++)
int 		verify_mempolicy(nodes->map[i], mode);
}

void main(){
	setup();
	cleanup();
}
