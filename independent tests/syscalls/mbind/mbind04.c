#include "incl.h"
#ifdef HAVE_NUMA_H
# include <numa.h>
# include <numaif.h>
# include "mbind.h"
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
}

void cleanup(void)
{
	tst_nodemap_free(nodes);
}

int  verify_policy(unsigned int node, int mode, unsigned flag)
{
	struct bitmask *bm = numa_allocate_nodemask();
	unsigned int i;
	void *ptr;
	pid_t pid;
	unsigned long size = PAGES_ALLOCATED * page_size;
	numa_bitmask_setbit(bm, node);
	ptr = tst_numa_map(NULL, size);
mbind(ptr, size, mode, bm->maskp, bm->size + 1, flag);
	numa_free_nodemask(bm);
	if (TST_RET) {
		tst_res(TFAIL | TTERRNO,
		        "mbind(%s, %s) node %u",
		        tst_mempolicy_mode_name(mode), mbind_flag_name(flag), node);
		return;
	}
	tst_res(TPASS, "mbind(%s, %s) node %u",
	        tst_mempolicy_mode_name(mode), mbind_flag_name(flag), node);
	const char *prefix = "child: ";
	pid = fork();
	if (pid) {
		prefix = "parent: ";
		tst_reap_children();
	}
	tst_nodemap_reset_counters(nodes);
	tst_numa_fault(ptr, size);
	tst_nodemap_count_pages(nodes, ptr, size);
	tst_numa_unmap(ptr, size);
	int fail = 0;
	for (i = 0; i < nodes->cnt; i++) {
		if (nodes->map[i] == node) {
			if (nodes->counters[i] == PAGES_ALLOCATED) {
				tst_res(TPASS, "%sNode %u allocated %u",
				        prefix, node, PAGES_ALLOCATED);
			} else {
				tst_res(TFAIL, "%sNode %u allocated %u, expected %u",
				        prefix, node, nodes->counters[i],
				        PAGES_ALLOCATED);
				fail = 1;
			}
			continue;
		}
		if (nodes->counters[i]) {
			tst_res(TFAIL, "%sNode %u allocated %u, expected 0",
			        prefix, i, nodes->counters[i]);
			fail = 1;
		}
	}
	if (fail)
		tst_nodemap_print_counters(nodes);
	if (!pid)
		exit(0);
}

const int modes[] = {
	MPOL_PREFERRED,
	MPOL_BIND,
	MPOL_INTERLEAVE,
};

int  verify_mbind(unsigned int n)
{
	unsigned int i;
	for (i = 0; i < nodes->cnt; i++) {
int 		verify_policy(nodes->map[i], modes[n], 0);
int 		verify_policy(nodes->map[i], modes[n], MPOL_MF_STRICT);
	}
}

void main(){
	setup();
	cleanup();
}
