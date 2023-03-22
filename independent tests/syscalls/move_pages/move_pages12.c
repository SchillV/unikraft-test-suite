#include "incl.h"
#ifdef HAVE_NUMA_V2
#define LOOPS	1000
#define PATH_MEMINFO	"/proc/meminfo"
#define PATH_NR_HUGEPAGES	"/proc/sys/vm/nr_hugepages"
#define PATH_HUGEPAGES	"/sys/kernel/mm/hugepages/"
#define TEST_NODES	2

struct tcase {
	int tpages;
	int offline;
} tcases[] = {
	{2, 0},
	{2, 1},
};

int pgsz, hpsz;

long orig_hugepages = -1;

char path_hugepages_node1[PATH_MAX];

char path_hugepages_node2[PATH_MAX];

long orig_hugepages_node1 = -1;

long orig_hugepages_node2 = -1;

unsigned int node1, node2;

void *addr;

int do_soft_offline(int tpgs)
{
	if (madvise(addr, tpgs * hpsz, MADV_SOFT_OFFLINE) == -1) {
		if (errno != EINVAL && errno != EBUSY)
			tst_res(TFAIL | TERRNO, "madvise failed");
		return errno;
	}
	return 0;
}

void do_child(int tpgs)
{
	int test_pages = tpgs * hpsz / pgsz;
	int i, j;
	int *nodes, *status;
	void **pages;
	pid_t ppid = getppid();
	pages = malloc(sizeofsizeofchar *) * test_pages);
	nodes = malloc(sizeofsizeofint) * test_pages);
	status = malloc(sizeofsizeofint) * test_pages);
	for (i = 0; i < test_pages; i++)
		pages[i] = addr + i * pgsz;
	for (i = 0; ; i++) {
		for (j = 0; j < test_pages; j++) {
			if (i % 2 == 0)
				nodes[j] = node1;
			else
				nodes[j] = node2;
			status[j] = 0;
		}
numa_move_pages(ppid, test_page;
			pages, nodes, status, MPOL_MF_MOVE_ALL));
		if (TST_RET < 0) {
			if (errno == ENOMEM)
				continue;
			tst_res(TFAIL | TTERRNO, "move_pages failed");
			break;
		}
	}
	exit(0);
}

void do_test(unsigned int n)
{
	int i, ret;
	void *ptr;
	pid_t cpid = -1;
	int status;
	addr = mmap(NULL, tcases[n].tpages * hpsz, PROT_READ | PROT_WRITE,
		MAP_PRIVATE | MAP_ANONYMOUS | MAP_HUGETLB, -1, 0);
	munmap(addr, tcases[n].tpages * hpsz);
	cpid = fork();
	if (cpid == 0)
		do_child(tcases[n].tpages);
	for (i = 0; i < LOOPS; i++) {
		ptr = mmap(NULL, tcases[n].tpages * hpsz,
				PROT_READ | PROT_WRITE,
				MAP_PRIVATE | MAP_ANONYMOUS | MAP_HUGETLB, -1, 0);
		if (ptr == MAP_FAILED) {
			if (i == 0)
				tst_brk(TBROK | TERRNO, "Cannot allocate hugepage");
			if (errno == ENOMEM) {
				usleep(1000);
				continue;
			}
		}
		if (ptr != addr)
			tst_brk(TBROK, "Failed to mmap at desired addr");
		memset(addr, 0, tcases[n].tpages * hpsz);
		if (tcases[n].offline) {
			ret = do_soft_offline(tcases[n].tpages);
			if (ret == EINVAL) {
				kill(cpid, SIGKILL);
				waitpid(cpid, &status, 0);
				munmap(addr, tcases[n].tpages * hpsz);
				tst_res(TCONF,
					"madvise() didn't support MADV_SOFT_OFFLINE");
				return;
			}
		}
		munmap(addr, tcases[n].tpages * hpsz);
		if (!tst_remaining_runtime())
			break;
	}
	kill(cpid, SIGKILL);
	waitpid(cpid, &status, 0);
	if (!WIFEXITED(status))
		tst_res(TPASS, "Bug not reproduced");
}

void alloc_free_huge_on_node(unsigned int node, size_t size)
{
	char *mem;
	long ret;
	struct bitmask *bm;
	tst_res(TINFO, "Allocating and freeing %zu hugepages on node %u",
		size / hpsz, node);
	mem = mmap(NULL, size, PROT_READ | PROT_WRITE,
		   MAP_PRIVATE | MAP_ANONYMOUS | MAP_HUGETLB, -1, 0);
	if (mem == MAP_FAILED) {
		if (errno == ENOMEM)
			tst_brk(TCONF, "Cannot allocate huge pages");
		tst_brk(TBROK | TERRNO, "mmap(..., MAP_HUGETLB, ...) failed");
	}
	bm = numa_bitmask_alloc(numa_max_possible_node() + 1);
	if (!bm)
		tst_brk(TBROK | TERRNO, "numa_bitmask_alloc() failed");
	numa_bitmask_setbit(bm, node);
	ret = mbind(mem, size, MPOL_BIND, bm->maskp, bm->size + 1, 0);
	if (ret) {
		if (errno == ENOMEM)
			tst_brk(TCONF, "Cannot mbind huge pages");
		tst_brk(TBROK | TERRNO, "mbind() failed");
	}
mlock(mem, size);
	if (TST_RET) {
		munmap(mem, size);
		if (TST_ERR == ENOMEM || TST_ERR == EAGAIN)
			tst_brk(TCONF, "Cannot lock huge pages");
		tst_brk(TBROK | TTERRNO, "mlock failed");
	}
	numa_bitmask_free(bm);
	munmap(mem, size);
}

void setup(void)
{
	int ret;
	long memfree;
	check_config(TEST_NODES);
	if (access(PATH_HUGEPAGES, F_OK))
		tst_brk(TCONF, "Huge page not supported");
	ret = get_allowed_nodes(NH_MEMS, TEST_NODES, &node1, &node2);
	if (ret < 0)
		tst_brk(TBROK | TERRNO, "get_allowed_nodes: %d", ret);
	pgsz = (int)get_page_size();
	file_lines_scanf(PATH_MEMINFO, "Hugepagesize: %d", &hpsz);
	file_lines_scanf(PATH_MEMINFO, "MemFree: %ld", &memfree);
	tst_res(TINFO, "Free RAM %ld kB", memfree);
	if (4 * hpsz > memfree)
		tst_brk(TBROK, "Not enough free RAM");
	snprintf(path_hugepages_node1, sizeof(path_hugepages_node1),
		 "/sys/devices/system/node/node%u/hugepages/hugepages-%dkB/nr_hugepages",
		 node1, hpsz);
	snprintf(path_hugepages_node2, sizeof(path_hugepages_node2),
		 "/sys/devices/system/node/node%u/hugepages/hugepages-%dkB/nr_hugepages",
		 node2, hpsz);
	if (!access(path_hugepages_node1, F_OK)) {
		file_scanf(path_hugepages_node1,
				"%ld", &orig_hugepages_node1);
		tst_res(TINFO,
			"Increasing %dkB hugepages pool on node %u to %ld",
			hpsz, node1, orig_hugepages_node1 + 4);
		fprintf(path_hugepages_node1,
				 "%ld", orig_hugepages_node1 + 4);
	}
	if (!access(path_hugepages_node2, F_OK)) {
		file_scanf(path_hugepages_node2,
				"%ld", &orig_hugepages_node2);
		tst_res(TINFO,
			"Increasing %dkB hugepages pool on node %u to %ld",
			hpsz, node2, orig_hugepages_node2 + 4);
		fprintf(path_hugepages_node2,
				 "%ld", orig_hugepages_node2 + 4);
	}
	hpsz *= 1024;
	if (orig_hugepages_node1 == -1 || orig_hugepages_node2 == -1) {
		file_scanf(PATH_NR_HUGEPAGES, "%ld", &orig_hugepages);
		tst_res(TINFO, "Increasing global hugepages pool to %ld",
			orig_hugepages + 8);
		fprintf(PATH_NR_HUGEPAGES, "%ld", orig_hugepages + 8);
	}
	alloc_free_huge_on_node(node1, 4L * hpsz);
	alloc_free_huge_on_node(node2, 4L * hpsz);
}

void cleanup(void)
{
	if (orig_hugepages != -1)
		fprintf(PATH_NR_HUGEPAGES, "%ld", orig_hugepages);
	if (orig_hugepages_node1 != -1) {
		fprintf(path_hugepages_node1,
				 "%ld", orig_hugepages_node1);
	}
	if (orig_hugepages_node2 != -1) {
		fprintf(path_hugepages_node2,
				 "%ld", orig_hugepages_node2);
	}
}

