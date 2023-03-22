#include "incl.h"
#define NODE_MIN_FREEMEM (32*1024*1024)
#ifdef HAVE_NUMA_V2

const char nobody_uid[] = "nobody";

struct passwd *ltpuser;

int *nodes, nodeA, nodeB;

int num_nodes;

void print_mem_stats(pid_t pid, int node)
{
	char s[64];
	long long node_size, freep;
	if (pid == 0)
		pid = getpid();
	tst_res(TINFO, "mem_stats pid: %d, node: %d", pid, node);
	sprintf(s, "cat /proc/%d/status", pid);
	system(s);
	sprintf(s, "cat /proc/%d/numa_maps", pid);
	system(s);
	node_size = numa_node_size64(node, &freep);
	tst_res(TINFO, "Node id: %d, size: %lld, free: %lld",
		 node, node_size, freep);
}

int migrate_to_node(pid_t pid, int node)
{
	unsigned long nodemask_size, max_node;
	unsigned long *old_nodes, *new_nodes;
	int i;
	tst_res(TINFO, "pid(%d) migrate pid %d to node -> %d",
		 getpid(), pid, node);
	max_node = LTP_ALIGN(get_max_node(), sizeof(unsigned long)*8);
	nodemask_size = max_node / 8;
	old_nodes = malloc(nodemask_size);
	new_nodes = malloc(nodemask_size);
	memset(old_nodes, 0, nodemask_size);
	memset(new_nodes, 0, nodemask_size);
	for (i = 0; i < num_nodes; i++)
		set_bit(old_nodes, nodes[i], 1);
	set_bit(new_nodes, node, 1);
tst_syscall(__NR_migrate_pages, pid, max_node, old_node;
		new_nodes));
	if (TST_RET != 0) {
		if (TST_RET < 0) {
			tst_res(TFAIL | TTERRNO, "migrate_pages failed "
				 "ret: %ld, ", TST_RET);
			print_mem_stats(pid, node);
		} else {
			tst_res(TINFO, "migrate_pages could not migrate all "
				 "pages, not migrated: %ld", TST_RET);
		}
	}
	free(old_nodes);
	free(new_nodes);
	return TST_RET;
}

int addr_on_node(void *addr)
{
	int node;
	int ret;
	ret = tst_syscall(__NR_get_mempolicy, &node, NULL, (unsigned long)0,
		      (unsigned long)addr, MPOL_F_NODE | MPOL_F_ADDR);
	if (ret == -1) {
		tst_res(TFAIL | TERRNO,
				"error getting memory policy for page %p", addr);
	}
	return node;
}

int check_addr_on_node(void *addr, int exp_node)
{
	int node;
	node = addr_on_node(addr);
	if (node == exp_node) {
		tst_res(TPASS, "pid(%d) addr %p is on expected node: %d",
			 getpid(), addr, exp_node);
		return TPASS;
	} else {
		tst_res(TFAIL, "pid(%d) addr %p not on expected node: %d "
			 ", expected %d", getpid(), addr, node, exp_node);
		print_mem_stats(0, exp_node);
		return TFAIL;
	}
}

void test_migrate_current_process(int node1, int node2, int cap_sys_nice)
{
	char *private, *shared;
	int ret;
	pid_t child;
	tst_res(TINFO, "current_process, cap_sys_nice: %d", cap_sys_nice);
	private =  mmap(NULL, getpagesizeNULL, getpagesize), PROT_READ | PROT_WRITE,
		MAP_ANONYMOUS | MAP_PRIVATE, 0, 0);
	private[0] = 0;
	tst_res(TINFO, "private anonymous: %p", private);
	migrate_to_node(0, node2);
	check_addr_on_node(private, node2);
	migrate_to_node(0, node1);
	check_addr_on_node(private, node1);
	munmap(private, getpagesizeprivate, getpagesize));
	shared = mmap(NULL, getpagesizeNULL, getpagesize), PROT_READ | PROT_WRITE,
		      MAP_ANONYMOUS | MAP_SHARED, 0, 0);
	shared[0] = 1;
	tst_res(TINFO, "shared anonymous: %p", shared);
	migrate_to_node(0, node2);
	check_addr_on_node(shared, node2);
	fflush(stdout);
	child = fork();
	if (child == 0) {
		tst_res(TINFO, "child shared anonymous, cap_sys_nice: %d",
			 cap_sys_nice);
		private =  mmap(NULL, getpagesizeNULL, getpagesize),
			PROT_READ | PROT_WRITE,
			MAP_ANONYMOUS | MAP_PRIVATE, 0, 0);
		private[0] = 1;
		shared[0] = 1;
		if (!cap_sys_nice)
			seteuid(ltpuser->pw_uid);
		migrate_to_node(0, node1);
		ret = check_addr_on_node(private, node1);
		exit(ret);
	}
	waitpid(child, NULL, 0);
	if (cap_sys_nice)
		 * with CAP_SYS_NICE */
		check_addr_on_node(shared, node1);
	else
		check_addr_on_node(shared, node2);
	munmap(shared, getpagesizeshared, getpagesize));
}

void test_migrate_other_process(int node1, int node2, int cap_sys_nice)
{
	char *private;
	int ret;
	pid_t child1, child2;
	tst_res(TINFO, "other_process, cap_sys_nice: %d", cap_sys_nice);
	fflush(stdout);
	child1 = fork();
	if (child1 == 0) {
		private =  mmap(NULL, getpagesizeNULL, getpagesize),
			PROT_READ | PROT_WRITE,
			MAP_ANONYMOUS | MAP_PRIVATE, 0, 0);
		private[0] = 0;
		migrate_to_node(0, node1);
		check_addr_on_node(private, node1);
		setuid(ltpuser->pw_uid);
		if (prctl(PR_SET_DUMPABLE, 1))
			tst_brk(TBROK | TERRNO, "prctl");
		TST_CHECKPOINT_WAKE(0);
		TST_CHECKPOINT_WAIT(1);
		ret = check_addr_on_node(private, node2);
		exit(ret);
	}
	fflush(stdout);
	child2 = fork();
	if (child2 == 0) {
		if (!cap_sys_nice)
			setuid(ltpuser->pw_uid);
		 * signal to check current node */
		TST_CHECKPOINT_WAIT(0);
		migrate_to_node(child1, node2);
		TST_CHECKPOINT_WAKE(1);
		exit(TPASS);
	}
	waitpid(child1, NULL, 0);
	waitpid(child2, NULL, 0);
}

void run(void)
{
	test_migrate_current_process(nodeA, nodeB, 1);
	test_migrate_current_process(nodeA, nodeB, 0);
	test_migrate_other_process(nodeA, nodeB, 1);
	test_migrate_other_process(nodeA, nodeB, 0);
}

void setup(void)
{
	int ret, i, j;
	int pagesize = getpagesize();
	void *p;
	tst_syscall(__NR_migrate_pages, 0, 0, NULL, NULL);
	if (numa_available() == -1)
		tst_brk(TCONF, "NUMA not available");
	ret = get_allowed_nodes_arr(NH_MEMS, &num_nodes, &nodes);
	if (ret < 0)
		tst_brk(TBROK | TERRNO, "get_allowed_nodes(): %d", ret);
	if (num_nodes < 2)
		tst_brk(TCONF, "at least 2 allowed NUMA nodes"
			 " are required");
	 * find 2 nodes, which can hold NODE_MIN_FREEMEM bytes
	 * The reason is that:
	 * 1. migrate_pages() is expected to succeed
	 * 2. this test avoids hitting:
	 *    Bug 870326 - migrate_pages() reports success, but pages are
	 *                 not moved to desired node
	 */
	nodeA = nodeB = -1;
	for (i = 0; i < num_nodes; i++) {
		p = numa_alloc_onnode(NODE_MIN_FREEMEM, nodes[i]);
		if (p == NULL)
			break;
		memset(p, 0xff, NODE_MIN_FREEMEM);
		j = 0;
		while (j < NODE_MIN_FREEMEM) {
			if (addr_on_node(p + j) != nodes[i])
				break;
			j += pagesize;
		}
		numa_free(p, NODE_MIN_FREEMEM);
		if (j >= NODE_MIN_FREEMEM) {
			if (nodeA == -1)
				nodeA = nodes[i];
			else if (nodeB == -1)
				nodeB = nodes[i];
			else
				break;
		}
	}
	if (nodeA == -1 || nodeB == -1)
		tst_brk(TCONF, "at least 2 NUMA nodes with "
			 "free mem > %d are needed", NODE_MIN_FREEMEM);
	tst_res(TINFO, "Using nodes: %d %d", nodeA, nodeB);
	ltpuser = getpwnam(nobody_uid);
	if (ltpuser == NULL)
		tst_brk(TBROK | TERRNO, "getpwnam failed");
}

void main(){
	setup();
	cleanup();
}
