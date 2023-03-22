#include "incl.h"
#if HAVE_NUMA_H
#endif
#ifdef HAVE_NUMA_V2
#define MEM_LENGTH (4 * 1024 * 1024)
#define UNKNOWN_POLICY -1
#define POLICY_DESC(x) .policy = x, .desc = #x
#define POLICY_DESC_TEXT(x, y) .policy = x, .desc = #x" ("y")"

struct bitmask *nodemask, *getnodemask, *empty_nodemask;

void test_default(unsigned int i, char *p);

void test_none(unsigned int i, char *p);

void test_invalid_nodemask(unsigned int i, char *p);

void check_policy_pref_or_local(int);
struct test_case {
	int policy;
	const char *desc;
	unsigned flags;
	int ret;
	int err;
	void (*check_policy)(int);
	void (*test)(unsigned int, char *);
	struct bitmask **exp_nodemask;
};

struct test_case tcase[] = {
	{
		POLICY_DESC(MPOL_DEFAULT),
		.ret = 0,
		.err = 0,
		.test = test_none,
		.exp_nodemask = &empty_nodemask,
	},
	{
		POLICY_DESC_TEXT(MPOL_DEFAULT, "target exists"),
		.ret = -1,
		.err = EINVAL,
		.test = test_default,
	},
	{
		POLICY_DESC_TEXT(MPOL_BIND, "no target"),
		.ret = -1,
		.err = EINVAL,
		.test = test_none,
	},
	{
		POLICY_DESC(MPOL_BIND),
		.ret = 0,
		.err = 0,
		.test = test_default,
		.exp_nodemask = &nodemask,
	},
	{
		POLICY_DESC_TEXT(MPOL_INTERLEAVE, "no target"),
		.ret = -1,
		.err = EINVAL,
		.test = test_none,
	},
	{
		POLICY_DESC(MPOL_INTERLEAVE),
		.ret = 0,
		.err = 0,
		.test = test_default,
		.exp_nodemask = &nodemask,
	},
	{
		POLICY_DESC_TEXT(MPOL_PREFERRED, "no target"),
		.ret = 0,
		.err = 0,
		.test = test_none,
		.check_policy = check_policy_pref_or_local,
	},
	{
		POLICY_DESC(MPOL_PREFERRED),
		.ret = 0,
		.err = 0,
		.test = test_default,
		.exp_nodemask = &nodemask,
	},
	{
		POLICY_DESC(MPOL_LOCAL),
		.ret = 0,
		.err = 0,
		.test = test_none,
		.exp_nodemask = &empty_nodemask,
		.check_policy = check_policy_pref_or_local,
	},
	{
		POLICY_DESC_TEXT(MPOL_LOCAL, "target exists"),
		.ret = -1,
		.err = EINVAL,
		.test = test_default,
	},
	{
		POLICY_DESC(UNKNOWN_POLICY),
		.ret = -1,
		.err = EINVAL,
		.test = test_none,
	},
	{
		POLICY_DESC_TEXT(MPOL_DEFAULT, "invalid flags"),
		.flags = -1,
		.ret = -1,
		.err = EINVAL,
		.test = test_none,
	},
	{
		POLICY_DESC_TEXT(MPOL_PREFERRED, "invalid nodemask"),
		.ret = -1,
		.err = EFAULT,
		.test = test_invalid_nodemask,
	},
};

void check_policy_pref_or_local(int policy)
{
	if (policy != MPOL_PREFERRED && policy != MPOL_LOCAL) {
		tst_res(TFAIL, "Wrong policy: %s(%d), "
			"expected MPOL_PREFERRED or MPOL_LOCAL",
			tst_mempolicy_mode_name(policy), policy);
	}
}

void test_default(unsigned int i, char *p)
{
	struct test_case *tc = &tcase[i];
mbind(p, MEM_LENGTH, tc->policy, nodemask->mask;
		   nodemask->size, tc->flags));
}

void test_none(unsigned int i, char *p)
{
	struct test_case *tc = &tcase[i];
mbind(p, MEM_LENGTH, tc->policy, NULL, 0, tc->flags);
}

void test_invalid_nodemask(unsigned int i, char *p)
{
	struct test_case *tc = &tcase[i];
mbind(p, MEM_LENGTH, tc->policy, sbrk(0) + 64*1024*102;
		   NUMA_NUM_NODES, tc->flags));
}

void setup(void)
{
	if (!is_numa(NULL, NH_MEMS, 1))
		tst_brk(TCONF, "requires NUMA with at least 1 node");
	empty_nodemask = numa_allocate_nodemask();
}

void setup_node(void)
{
	int test_node = -1;
	if (get_allowed_nodes(NH_MEMS, 1, &test_node) < 0)
		tst_brk(TBROK | TERRNO, "get_allowed_nodes failed");
	nodemask = numa_allocate_nodemask();
	getnodemask = numa_allocate_nodemask();
	numa_bitmask_setbit(nodemask, test_node);
}

void do_test(unsigned int i)
{
	struct test_case *tc = &tcase[i];
	int policy, fail = 0;
	char *p = NULL;
	tst_res(TINFO, "case %s", tc->desc);
	if (tc->policy == MPOL_LOCAL) {
		if ((tst_kvercmp(5, 14, 0)) >= 0)
			tc->check_policy = NULL;
	}
	setup_node();
	p = mmap(NULL, MEM_LENGTH, PROT_READ | PROT_WRITE, MAP_PRIVATE |
			 MAP_ANONYMOUS, 0, 0);
	tc->test(i, p);
	if (TST_RET >= 0) {
get_mempolicy(&policy, getnodemask->mask;
				   getnodemask->size, p, MPOL_F_ADDR));
		if (TST_RET < 0) {
			tst_res(TFAIL | TTERRNO, "get_mempolicy failed");
			return;
		}
		if (tc->check_policy)
			tc->check_policy(policy);
		else if (tc->policy != policy) {
			tst_res(TFAIL, "Wrong policy: %s(%d), expected: %s(%d)",
				tst_mempolicy_mode_name(policy), policy,
				tst_mempolicy_mode_name(tc->policy), tc->policy);
			fail = 1;
		}
		if (tc->exp_nodemask) {
			struct bitmask *exp_mask = *(tc->exp_nodemask);
			if (!numa_bitmask_equal(exp_mask, getnodemask)) {
				tst_res(TFAIL, "masks are not equal");
				tst_res_hexd(TINFO, exp_mask->maskp,
					exp_mask->size / 8, "exp_mask: ");
				tst_res_hexd(TINFO, getnodemask->maskp,
					getnodemask->size / 8, "returned: ");
				fail = 1;
			}
		}
	}
	if (TST_RET != tc->ret) {
		tst_res(TFAIL, "wrong return code: %ld, expected: %d",
			TST_RET, tc->ret);
		fail = 1;
	}
	if (TST_RET == -1 && TST_ERR != tc->err) {
		tst_res(TFAIL | TTERRNO, "expected errno: %s, got",
			tst_strerrno(tc->err));
		fail = 1;
	}
	if (!fail)
		tst_res(TPASS, "Test passed");
}

void main(){
	setup();
	cleanup();
}
