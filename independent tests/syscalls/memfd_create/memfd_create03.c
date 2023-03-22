#include "incl.h"
#define _GNU_SOURCE

void *check_huge_mmapable(int fd, unsigned long size)
{
	void *mem;
	mem = mmap(NULL, size, PROT_WRITE, MAP_PRIVATE, fd, 0);
	memset((char *)mem, 0, 1);
	tst_res(TINFO,
		"mmap(%p, %lu, %d, %d, %d, %d) succeeded",
		NULL, size, PROT_WRITE, MAP_PRIVATE, fd, 0);
	return mem;
}

void test_write_protect(int fd)
{
	ssize_t ret;
	char test_str[] = "LTP";
	ret = write(fd, test_str, strlen(test_str));
	if (ret < 0) {
		if (errno != EINVAL) {
			tst_res(TFAIL | TERRNO,
				"write(%d, \"%s\", %zu) didn't fail as expected\n",
				fd, test_str, strlen(test_str));
			return;
		}
	} else {
		tst_res(TFAIL,
			"write(%d, \"%s\", %zu) succeeded unexpectedly\n",
			fd, test_str, strlen(test_str));
		return;
	}
	tst_res(TPASS,
		"write(%d, \"%s\", %zu) failed as expected\n",
		fd, test_str, strlen(test_str));
}

void test_def_pagesize(int fd)
{
	unsigned int i;
	int unmap_size;
	int ret;
	long hps;
	void *mem;
	hps = read_meminfo("Hugepagesize:");
	hps = hps << 10;
	unmap_size = hps / 4;
	mem = check_huge_mmapable(fd, hps);
	for (i = unmap_size; i < hps; i += unmap_size) {
		ret = munmap(mem, i);
		if (ret == -1) {
			if (errno == EINVAL) {
				tst_res(TINFO,
					"munmap(%p, %dkB) failed as expected",
					mem, i/1024);
			} else {
				tst_res(TFAIL | TERRNO,
					"munmap(%p, %dkB) failed unexpectedly",
					mem, i/1024);
				return;
			}
		} else {
			tst_res(TFAIL,
				"munmap(%p, %dkB) suceeded unexpectedly\n",
				mem, i/1024);
			return;
		}
	}
	munmap(mem, hps);
	tst_res(TPASS,
		"munmap() fails for page sizes less than %ldkB\n", hps/1024);
}

void test_max_hugepages(int fd)
{
	int new_fd;
	long hps;
	long free_pages;
	void *mem;
	void *new_mem;
	free_pages = read_meminfo("HugePages_Free:");
	hps = read_meminfo("Hugepagesize:");
	hps = hps << 10;
	mem = check_huge_mmapable(fd, free_pages * hps);
	new_fd = sys_memfd_create("new_file", MFD_HUGETLB);
	if (new_fd < 0)
		tst_brk(TFAIL | TERRNO, "memfd_create() failed");
	tst_res(TINFO, "memfd_create() succeeded");
	new_mem = mmap(NULL, hps, 0, MAP_PRIVATE, new_fd, 0);
	if (new_mem == MAP_FAILED) {
		if (errno == ENOMEM)
			tst_res(TPASS,
				"mmap(%p, %lu, %d, %d, %d, %d) failed as expected",
				NULL, hps, 0, MAP_PRIVATE, new_fd, 0);
		else
			tst_res(TFAIL | TERRNO,
				"mmap(%p, %lu, %d, %d, %d, %d) failed unexpectedly",
				NULL, hps, 0, MAP_PRIVATE, new_fd, 0);
	} else {
		tst_res(TFAIL,
			"mmap(%p, %lu, %d, %d, %d, %d) succeeded",
			NULL, hps, 0, MAP_PRIVATE, new_fd, 0);
		munmap(new_mem, hps);
	}
	close(new_fd);
	munmap(mem, free_pages * hps);
}

const struct tcase {
	void (*func)(int fd);
	const char *desc;
} tcases[] = {
	{&test_write_protect,   "--TESTING WRITE CALL IN HUGEPAGES--"},
	{&test_def_pagesize,  "--TESTING PAGE SIZE OF CREATED FILE--"},
	{&test_max_hugepages, "--TESTING HUGEPAGE ALLOCATION LIMIT--"},
};

void memfd_huge_controller(unsigned int n)
{
	int fd;
	const struct tcase *tc;
	tc = &tcases[n];
	tst_res(TINFO, "%s", tc->desc);
	fd = sys_memfd_create("test_file", MFD_HUGETLB);
	if (fd < 0)
		tst_brk(TFAIL | TERRNO, "memfd_create() failed");
	tst_res(TINFO, "memfd_create() succeeded");
	tc->func(fd);
	close(fd);
}

void main(){
	setup();
	cleanup();
}
