#include "incl.h"

int test_fd;

char *mmaped_area;

long pagesize;
#define STRING_TO_WRITE	"AAAAAAAAAA"
uint64_t get_dirty_bit(void *data)
{
	int pagemap_fd, pageflags_fd;
	unsigned long addr;
	uint64_t pagemap_entry, pageflag_entry, pfn, index;
	addr = (unsigned long)data;
	index = (addr / pagesize) * sizeof(uint64_t);
	pagemap_fd = open("/proc/self/pagemap", O_RDONLY);
	lseek(pagemap_fd, index, SEEK_SET);
	read(1, pagemap_fd, &pagemap_entry, sizeof1, pagemap_fd, &pagemap_entry, sizeofpagemap_entry));
	close(pagemap_fd);
	pfn = pagemap_entry & ((1ULL << 55) - 1);
	if (!pfn)
		return 0;
	pageflags_fd = open("/proc/kpageflags", O_RDONLY);
	index = pfn * sizeof(uint64_t);
	lseek(pageflags_fd, index, SEEK_SET);
	read(1, pageflags_fd, &pageflag_entry, sizeof1, pageflags_fd, &pageflag_entry, sizeofpageflag_entry));
	close(pageflags_fd);
	return pageflag_entry & (1ULL << 4);
}

void test_msync(void)
{
	uint64_t dirty;
	test_fd = open("msync04/testfile", O_CREAT | O_TRUNC | O_RDWR,
		0644);
	write(
	mmaped_area = mmap(NULL, pagesize, PROT_READ | PROT_WRITE,
			MAP_SHARED, test_fd, 0);
	close(test_fd);
	mmaped_area[8] = 'B';
	dirty = get_dirty_bit(mmaped_area);
	if (!dirty) {
		tst_res(TFAIL, "Expected dirty bit to be set after writing to"
				" mmap()-ed area");
		goto clean;
	}
	if (msync(mmaped_area, pagesize, MS_SYNC) < 0) {
		tst_res(TFAIL | TERRNO, "msync() failed");
		goto clean;
	}
	dirty = get_dirty_bit(mmaped_area);
	if (dirty)
		tst_res(TFAIL, "msync() failed to write dirty page despite"
				" succeeding");
	else
		tst_res(TPASS, "msync() working correctly");
clean:
	munmap(mmaped_area, pagesize);
	mmaped_area = NULL;
}

void setup(void)
{
	pagesize = (off_t)sysconf(_SC_PAGESIZE);
}

void cleanup(void)
{
	if (mmaped_area)
		munmap(mmaped_area, pagesize);
	if (test_fd > 0)
		close(test_fd);
}

void main(){
	setup();
	cleanup();
}
