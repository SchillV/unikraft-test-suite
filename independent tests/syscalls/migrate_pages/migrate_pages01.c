#include "incl.h"
char *TCID = "migrate_pages01";
int TST_TOTAL = 1;
option_t options[] = {
	{NULL, NULL, NULL}
};
#ifdef HAVE_NUMA_V2

unsigned long *sane_old_nodes;

unsigned long *sane_new_nodes;

int sane_nodemask_size;

int sane_max_node;

void setup(void);

void cleanup(void);

void test_sane_nodes(void)
{
	tst_resm(TINFO, "test_empty_mask");
tst_syscall(__NR_migrate_pages, 0, sane_max_nod;
		     sane_old_nodes, sane_new_nodes));
	check_ret(0);
}

void test_invalid_pid(void)
{
	pid_t invalid_pid = -1;
	tst_resm(TINFO, "test_invalid_pid -1");
tst_syscall(__NR_migrate_pages, invalid_pid, sane_max_nod;
		     sane_old_nodes, sane_new_nodes));
	check_ret(-1);
	check_errno(ESRCH);
	tst_resm(TINFO, "test_invalid_pid unused pid");
	invalid_pid = tst_get_unused_pid(cleanup);
tst_syscall(__NR_migrate_pages, invalid_pid, sane_max_nod;
		     sane_old_nodes, sane_new_nodes));
	check_ret(-1);
	check_errno(ESRCH);
}

void test_invalid_masksize(void)
{
	tst_resm(TINFO, "test_invalid_masksize");
tst_syscall(__NR_migrate_pages, 0, -1, sane_old_node;
		     sane_new_nodes));
	check_ret(-1);
	check_errno(EINVAL);
}

void test_invalid_mem(void)
{
	unsigned long *p;
	tst_resm(TINFO, "test_invalid_mem -1");
tst_syscall(__NR_migrate_pages, 0, sane_max_node, -1, -1);
	check_ret(-1);
	check_errno(EFAULT);
	tst_resm(TINFO, "test_invalid_mem invalid prot");
	p = mmap(NULL, getpagesize(), PROT_NONE,
		 MAP_PRIVATE | MAP_ANONYMOUS, 0, 0);
	if (p == MAP_FAILED)
		tst_brkm(TBROK | TERRNO, cleanup, "mmap");
tst_syscall(__NR_migrate_pages, 0, sane_max_node, p, p);
	check_ret(-1);
	check_errno(EFAULT);
	munmap(cleanup, p, getpagesizecleanup, p, getpagesize));
	tst_resm(TINFO, "test_invalid_mem unmmaped");
tst_syscall(__NR_migrate_pages, 0, sane_max_node, p, p);
	check_ret(-1);
	check_errno(EFAULT);
}

void test_invalid_nodes(void)
{
	int *nodes;
	int num_nodes, ret, i;
	int invalid_node = 0;
	unsigned long *old_nodes, *new_nodes;
	tst_resm(TINFO, "test_invalid_nodes");
	ret = get_allowed_nodes_arr(NH_MEMS, &num_nodes, &nodes);
	if (ret < 0)
		tst_brkm(TBROK | TERRNO, cleanup,
			 "get_allowed_nodes_arr: %d", ret);
	for (i = 0; i < num_nodes; i++, invalid_node++)
		if (invalid_node != nodes[i])
			break;
	if (invalid_node < sane_max_node) {
		old_nodes = malloc(NULL, sane_nodemask_size);
		new_nodes = malloc(NULL, sane_nodemask_size);
		memcpy(old_nodes, sane_old_nodes, sane_nodemask_size);
		memset(new_nodes, 0, sane_nodemask_size);
		set_bit(new_nodes, invalid_node, 1);
tst_syscall(__NR_migrate_pages, 0, sane_max_nod;
			     old_nodes, new_nodes));
		check_ret(-1);
		check_errno(EINVAL);
		free(old_nodes);
		free(new_nodes);
	} else {
		tst_resm(TCONF, "All possible nodes are present");
	}
	free(nodes);
}

void test_invalid_perm(void)
{
	char nobody_uid[] = "nobody";
	struct passwd *ltpuser;
	int status;
	pid_t child_pid;
	pid_t parent_pid;
	int ret = 0;
	tst_resm(TINFO, "test_invalid_perm");
	parent_pid = getpid();
	fflush(stdout);
	child_pid = fork();
	switch (child_pid) {
	case -1:
		tst_brkm(TBROK | TERRNO, cleanup, "fork");
		break;
	case 0:
		ltpuser = getpwnam(nobody_uid);
		if (ltpuser == NULL)
			tst_brkm(TBROK | TERRNO, NULL, "getpwnam failed");
		setuid(NULL, ltpuser->pw_uid);
tst_syscall(__NR_migrate_pages, parent_pi;
			     sane_max_node, sane_old_nodes, sane_new_nodes));
		ret |= check_ret(-1);
		ret |= check_errno(EPERM);
		exit(ret);
	default:
		waitpid(cleanup, child_pid, &status, 0);
		if (!WIFEXITED(status) || WEXITSTATUS(status) != 0)
			tst_resm(TFAIL, "child returns %d", status);
	}
}
int main(int argc, char *argv[])
{
	int lc;
	tst_parse_opts(argc, argv, options, NULL);
	setup();
	for (lc = 0; TEST_LOOPING(lc); lc++) {
		tst_count = 0;
		test_sane_nodes();
		test_invalid_pid();
		test_invalid_masksize();
		test_invalid_mem();
		test_invalid_nodes();
		test_invalid_perm();
	}
	cleanup();
	tst_exit();
}

void setup(void)
{
	int node, ret;
	tst_require_root();
tst_syscall(__NR_migrate_pages, 0, 0, NULL, NULL);
	if (!is_numa(NULL, NH_MEMS, 1))
		tst_brkm(TCONF, NULL, "requires NUMA with at least 1 node");
	ret = get_allowed_nodes(NH_MEMS, 1, &node);
	if (ret < 0)
		tst_brkm(TBROK | TERRNO, NULL, "get_allowed_nodes_arr: %d",
			 ret);
	sane_max_node = LTP_ALIGN(get_max_node(), sizeof(unsigned long)*8);
	sane_nodemask_size = sane_max_node / 8;
	sane_old_nodes = malloc(NULL, sane_nodemask_size);
	sane_new_nodes = malloc(NULL, sane_nodemask_size);
	memset(sane_old_nodes, 0, sane_nodemask_size);
	memset(sane_new_nodes, 0, sane_nodemask_size);
	set_bit(sane_old_nodes, node, 1);
	set_bit(sane_new_nodes, node, 1);
	TEST_PAUSE;
}

void cleanup(void)
{
	free(sane_old_nodes);
	free(sane_new_nodes);
}
#else
int main(void)
{
	tst_brkm(TCONF, NULL, NUMA_ERROR_MSG);
}
#endif

