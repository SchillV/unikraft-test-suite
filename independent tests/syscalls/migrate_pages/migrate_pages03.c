#include "incl.h"
#ifdef HAVE_NUMA_V2
#define N_PAGES 20
#define N_LOOPS 600
#define TEST_NODES 2

int orig_ksm_run = -1;

unsigned int page_size;

void *test_pages[N_PAGES];

int num_nodes, max_node;

int *nodes;

unsigned long *new_nodes[2];

const char nobody_uid[] = "nobody";

struct passwd *ltpuser;

void setup(void)
{
	int n;
	unsigned long nodemask_size;
	if (access(PATH_KSM, F_OK))
		tst_brk(TCONF, "KSM configuration was not enabled");
	if (get_allowed_nodes_arr(NH_MEMS, &num_nodes, &nodes) < 0)
		tst_brk(TBROK | TERRNO, "get_allowed_nodes() failed");
	if (num_nodes < TEST_NODES) {
		tst_brk(TCONF, "requires NUMA with at least %d node",
			TEST_NODES);
	}
	ltpuser = getpwnam(nobody_uid);
	max_node = LTP_ALIGN(get_max_node(), sizeof(unsigned long) * 8);
	nodemask_size = max_node / 8;
	new_nodes[0] = malloc(nodemask_size);
	new_nodes[1] = malloc(nodemask_size);
	memset(new_nodes[0], 0, nodemask_size);
	memset(new_nodes[1], 0, nodemask_size);
	set_bit(new_nodes[0], nodes[0], 1);
	set_bit(new_nodes[1], nodes[1], 1);
	page_size = getpagesize();
	for (n = 0; n < N_PAGES; n++) {
		test_pages[n] = mmap(NULL, page_size, PROT_READ | PROT_WRITE | PROT_EXEC,
					  MAP_PRIVATE | MAP_ANONYMOUS, -1, 0);
		if (madvise(test_pages[n], page_size, MADV_MERGEABLE)) {
			if (errno == EINVAL) {
				tst_brk(TCONF | TERRNO, "madvise() didn't "
					"support MADV_MERGEABLE");
			}
			tst_brk(TBROK | TERRNO,
				"madvise(MADV_MERGEABLE) failed");
		}
		if (mbind(test_pages[n], page_size, MPOL_BIND, new_nodes[0],
			  max_node, 0))
			tst_brk(TBROK | TERRNO, "mbind(MPOL_BIND) failed");
		memset(test_pages[n], 0, page_size);
	}
	file_scanf(PATH_KSM "run", "%d", &orig_ksm_run);
	fprintf(PATH_KSM "run", "%d", 1);
	wait_ksmd_full_scan();
}

void cleanup(void)
{
	int n;
	for (n = 0; n < N_PAGES; n++) {
		if (test_pages[n])
			munmap(test_pages[n], page_size);
	}
	free(new_nodes[0]);
	free(new_nodes[1]);
	if (orig_ksm_run != -1)
		fprintf(PATH_KSM "run", "%d", orig_ksm_run);
}

void migrate_test(void)
{
	int loop, i, ret;
	seteuid(ltpuser->pw_uid);
	for (loop = 0; loop < N_LOOPS; loop++) {
		i = loop % 2;
		ret = tst_syscall(__NR_migrate_pages, 0, max_node,
				   new_nodes[i], new_nodes[i ? 0 : 1]);
		if (ret < 0) {
			tst_res(TFAIL | TERRNO, "migrate_pages() failed");
			return;
		}
		if (!tst_remaining_runtime()) {
			tst_res(TINFO, "Out of runtime, exiting...");
			break;
		}
	}
	seteuid(0);
	tst_res(TPASS, "migrate_pages() passed");
}

