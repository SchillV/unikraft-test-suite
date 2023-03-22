#include "incl.h"
#ifdef HAVE_NUMA_V2
#define PAGES_ALLOCATED 16u
#define POLICY_DESC_TEXT(x, y) .policy = x, .desc = "policy: "#x", "y

struct tst_nodemap *node;

struct bitmask *nodemask;
struct test_case {
	int policy;
	const char *desc;
	unsigned int flags;
	int err;
	char *addr;
};

struct test_case tcase[] = {
	{
		POLICY_DESC_TEXT(MPOL_DEFAULT, "invalid address"),
		.addr = NULL,
		.err = EFAULT,
		.flags = MPOL_F_ADDR,
	},
	{
		POLICY_DESC_TEXT(MPOL_DEFAULT, "invalid flags, no target"),
		.err = EINVAL,
		.flags = -1,
	},
};

void setup(void)
{
	node = tst_get_nodemap(TST_NUMA_MEM, PAGES_ALLOCATED * getpagesize() / 1024);
	if (node->cnt < 1)
		tst_brk(TCONF, "test requires at least one NUMA memory node");
	nodemask = numa_allocate_nodemask();
}

void cleanup(void)
{
	numa_free_nodemask(nodemask);
	tst_nodemap_free(node);
}

void do_test(unsigned int i)
{
	struct test_case *tc = &tcase[i];
	int policy;
	TST_EXP_FAIL(get_mempolicy(&policy, nodemask->maskp, nodemask->size,
				   tc->addr, tc->flags), tc->err, "%s", tc->desc);
}

void main(){
	setup();
	cleanup();
}
