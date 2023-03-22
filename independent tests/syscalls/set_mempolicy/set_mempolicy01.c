#include "incl.h"
#ifdef HAVE_NUMA_V2
# include <numa.h>
# include <numaif.h>
#endif
#ifdef HAVE_NUMA_V2

size_t page_size;

struct tst_nodemap *nodes;
#define PAGES_ALLOCATED 16u

void setup(void)
{
	page_size = getpagesize();
	nodes = tst_get_nodemap(TST_NUMA_MEM, 2 * PAGES_ALLOCATED * page_size / 1024);
	if (nodes->cnt <= 1)
		tst_brk(TCONF, "Test requires at least two NUMA memory nodes");
	 * In most cases, set_mempolicy01 finish quickly, but when the platform
	 * has multiple NUMA nodes, the test matrix combination grows exponentially
	 * and bring about test time to increase extremely fast.
	 *
	 * Here reset the maximum runtime according to the NUMA nodes.
	 */
	tst_set_max_runtime(test.max_runtime * (1 << nodes->cnt/16));
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
	const char *prefix = "child: ";
	if (fork()) {
		prefix = "parent: ";
		tst_reap_children();
	}
	tst_nodemap_reset_counters(nodes);
	alloc_fault_count(nodes, NULL, PAGES_ALLOCATED * page_size);
	tst_nodemap_print_counters(nodes);
	for (i = 0; i < nodes->cnt; i++) {
		if (nodes->map[i] == node) {
			if (nodes->counters[i] == PAGES_ALLOCATED) {
				tst_res(TPASS, "%sNode %u allocated %u",
				        prefix, node, PAGES_ALLOCATED);
			} else {
				tst_res(TFAIL, "%sNode %u allocated %u, expected %u",
				        prefix, node, nodes->counters[i],
				        PAGES_ALLOCATED);
			}
			continue;
		}
		if (nodes->counters[i]) {
			tst_res(TFAIL, "%sNode %u allocated %u, expected 0",
			        prefix, i, nodes->counters[i]);
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
