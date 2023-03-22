#include "incl.h"
#ifdef HAVE_NUMA_V2
# include <numa.h>
# include <numaif.h>
#endif
#define MNTPOINT "mntpoint"
#define FILES 10
#ifdef HAVE_NUMA_V2

size_t page_size;

struct tst_nodemap *nodes;

void setup(void)
{
	int node_min_pages = FILES * (FILES + 1) / 2 * 10 + FILES * 10;
	page_size = getpagesize();
	nodes = tst_get_nodemap(TST_NUMA_MEM, node_min_pages * page_size / 1024);
	if (nodes->cnt <= 1)
		tst_brk(TCONF, "Test requires at least two NUMA memory nodes");
}

void cleanup(void)
{
	tst_nodemap_free(nodes);
}

void alloc_and_check(void)
{
	unsigned int i, j;
	char path[1024];
	unsigned int total_pages = 0;
	unsigned int sum_pages = 0;
	tst_nodemap_reset_counters(nodes);
	 * The inner loop loops node->cnt times to ensure the sum could
	 * be evenly distributed among the nodes.
	 */
	for (i = 1; i <= FILES; i++) {
		for (j = 1; j <= nodes->cnt; j++) {
			size_t size = 10 * i + j % 10;
			snprintf(path, sizeof(path), MNTPOINT "/numa-test-file-%i-%i", i, j);
			alloc_fault_count(nodes, path, size * page_size);
			total_pages += size;
		}
	}
	for (i = 0; i < nodes->cnt; i++) {
		float min_pages = 1.00 * total_pages / nodes->cnt - treshold;
		float max_pages = 1.00 * total_pages / nodes->cnt + treshold;
		if (nodes->counters[i] > min_pages && nodes->counters[i] < max_pages) {
			tst_res(TPASS, "Node %u allocated %u <%.2f,%.2f>",
			        nodes->map[i], nodes->counters[i], min_pages, max_pages);
		} else {
			tst_res(TFAIL, "Node %u allocated %u, expected <%.2f,%.2f>",
			        nodes->map[i], nodes->counters[i], min_pages, max_pages);
		}
		sum_pages += nodes->counters[i];
	}
	if (sum_pages != total_pages) {
		tst_res(TFAIL, "Sum of nodes %u != allocated pages %u",
		        sum_pages, total_pages);
		return;
	}
	tst_res(TPASS, "Sum of nodes equals to allocated pages (%u)", total_pages);
}

int  verify_set_mempolicy(
{
	struct bitmask *bm = numa_allocate_nodemask();
	unsigned int alloc_on_nodes = nodes->cnt;
	unsigned int i;
	for (i = 0; i < alloc_on_nodes; i++)
		numa_bitmask_setbit(bm, nodes->map[i]);
set_mempolicy(MPOL_INTERLEAVE, bm->maskp, bm->size+1);
	if (TST_RET) {
		tst_res(TFAIL | TTERRNO,
		        "set_mempolicy(MPOL_INTERLEAVE)");
		return;
	}
	tst_res(TPASS, "set_mempolicy(MPOL_INTERLEAVE)");
	alloc_and_check();
	numa_free_nodemask(bm);
}

void main(){
	setup();
	cleanup();
}
