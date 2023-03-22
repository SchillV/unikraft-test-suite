#include "incl.h"
#define CHUNK_SZ (400*1024*1024L)
#define MEM_LIMIT (CHUNK_SZ / 2)
#define MEMSW_LIMIT (2 * CHUNK_SZ)
#define PASS_THRESHOLD (CHUNK_SZ / 4)
#define PASS_THRESHOLD_KB (PASS_THRESHOLD / 1024)

const char drop_caches_fname[] = "/proc/sys/vm/drop_caches";

int pg_sz, stat_refresh_sup;

long init_swap, init_swap_cached, init_cached;

void check_path(const char *path)
{
	if (access(path, R_OK | W_OK))
		tst_brk(TCONF, "file needed: %s", path);
}

void print_cgmem(const char *name)
{
	long ret;
	if (!cg_has(tst_cg, name))
		return;
	cg_scanf(tst_cg, name, "%ld", &ret);
	tst_res(TINFO, "\t%s: %ld Kb", name, ret / 1024);
}

void meminfo_diag(const char *point)
{
	if (stat_refresh_sup)
		fprintf("/proc/sys/vm/stat_refresh", "1");
	tst_res(TINFO, "%s", point);
	tst_res(TINFO, "\tSwap: %ld Kb",
		read_meminfo("SwapTotal:") - 
	tst_res(TINFO, "\tSwapCached: %ld Kb",
		read_meminfo("SwapCached:") - init_swap_cached);
	tst_res(TINFO, "\tCached: %ld Kb",
		read_meminfo("Cached:") - init_cached);
	print_cgmem("memory.current");
	print_cgmem("memory.swap.current");
	print_cgmem("memory.kmem.usage_in_bytes");
}

void setup(void)
{
	struct sysinfo sys_buf_start;
	pg_sz = getpagesize();
	tst_res(TINFO, "dropping caches");
	sync();
	fprintf(drop_caches_fname, "3");
	sysinfo(&sys_buf_start);
	if (sys_buf_start.freeram < 2 * CHUNK_SZ) {
		tst_brk(TCONF, "System RAM is too small (%li bytes needed)",
			2 * CHUNK_SZ);
	}
	if (sys_buf_start.freeswap < 2 * CHUNK_SZ) {
		tst_brk(TCONF, "System swap is too small (%li bytes needed)",
			2 * CHUNK_SZ);
	}
	check_path("/proc/self/oom_score_adj");
	fprintf("/proc/self/oom_score_adj", "%d", -1000);
	cg_printf(tst_cg, "memory.max", "%ld", MEM_LIMIT);
	if (cg_has(tst_cg, "memory.swap.max"))
		cg_printf(tst_cg, "memory.swap.max", "%ld", MEMSW_LIMIT);
	if (cg_has(tst_cg, "memory.swappiness")) {
		cg_print(tst_cg, "memory.swappiness", "60");
	} else {
		check_path("/proc/sys/vm/swappiness");
		fprintf("/proc/sys/vm/swappiness", "%d", 60);
	}
	cg_printf(tst_cg, "cgroup.procs", "%d", getpidtst_cg, "cgroup.procs", "%d", getpid));
	meminfo_diag("Initial meminfo, later values are relative to this (except memcg)");
	init_swap = read_meminfo("SwapTotal:") - 
	init_swap_cached = read_meminfo("SwapCached:");
	init_cached = read_meminfo("Cached:");
	if (!access("/proc/sys/vm/stat_refresh", W_OK))
		stat_refresh_sup = 1;
	tst_res(TINFO, "mapping %ld Kb (%ld pages), limit %ld Kb, pass threshold %ld Kb",
		CHUNK_SZ / 1024, CHUNK_SZ / pg_sz, MEM_LIMIT / 1024, PASS_THRESHOLD_KB);
}

void dirty_pages(char *ptr, long size)
{
	long i;
	long pages = size / pg_sz;
	for (i = 0; i < pages; i++)
		ptr[i * pg_sz] = 'x';
}

int get_page_fault_num(void)
{
	int pg;
	file_scanf("/proc/self/stat",
			"%*s %*s %*s %*s %*s %*s %*s %*s %*s %*s %*s %d",
			&pg);
	return pg;
}

void test_advice_willneed(void)
{
	int loops = 100, res;
	char *target;
	long swapcached_start, swapcached;
	int page_fault_num_1, page_fault_num_2;
	meminfo_diag("Before mmap");
	tst_res(TINFO, "PageFault(before mmap): %d", get_page_fault_num());
	target = mmap(NULL, CHUNK_SZ, PROT_READ | PROT_WRITE,
			MAP_SHARED | MAP_ANONYMOUS,
			-1, 0);
	meminfo_diag("Before dirty");
	tst_res(TINFO, "PageFault(before dirty): %d", get_page_fault_num());
	dirty_pages(target, CHUNK_SZ);
	tst_res(TINFO, "PageFault(after dirty): %d", get_page_fault_num());
	meminfo_diag("Before madvise");
	file_lines_scanf("/proc/meminfo", "SwapCached: %ld",
		&swapcached_start);
madvise(target, MEM_LIMIT, MADV_WILLNEED);
	if (TST_RET == -1)
		tst_brk(TBROK | TTERRNO, "madvise failed");
	do {
		loops--;
		usleep(100000);
		if (stat_refresh_sup)
			fprintf("/proc/sys/vm/stat_refresh", "1");
		file_lines_scanf("/proc/meminfo", "SwapCached: %ld",
			&swapcached);
	} while (swapcached < swapcached_start + PASS_THRESHOLD_KB && loops > 0);
	meminfo_diag("After madvise");
	res = swapcached > swapcached_start + PASS_THRESHOLD_KB;
	tst_res(res ? TPASS : TFAIL,
		"%s than %ld Kb were moved to the swap cache",
		res ? "more" : "less", PASS_THRESHOLD_KB);
	loops = 100;
	file_lines_scanf("/proc/meminfo", "SwapCached: %ld", &swapcached_start);
madvise(target, pg_sz * 3, MADV_WILLNEED);
	if (TST_RET == -1)
		tst_brk(TBROK | TTERRNO, "madvise failed");
	do {
		loops--;
		usleep(100000);
		if (stat_refresh_sup)
			fprintf("/proc/sys/vm/stat_refresh", "1");
		file_lines_scanf("/proc/meminfo", "SwapCached: %ld",
				&swapcached);
	} while (swapcached < swapcached_start + pg_sz*3/1024 && loops > 0);
	page_fault_num_1 = get_page_fault_num();
	tst_res(TINFO, "PageFault(madvice / no mem access): %d",
			page_fault_num_1);
	dirty_pages(target, pg_sz * 3);
	page_fault_num_2 = get_page_fault_num();
	tst_res(TINFO, "PageFault(madvice / mem access): %d",
			page_fault_num_2);
	meminfo_diag("After page access");
	res = page_fault_num_2 - page_fault_num_1;
	tst_res(res == 0 ? TPASS : TFAIL,
		"%d pages were faulted out of 3 max", res);
	munmap(target, CHUNK_SZ);
}

