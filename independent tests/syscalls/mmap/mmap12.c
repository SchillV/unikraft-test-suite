#include "incl.h"
#define TEMPFILE        "mmapfile"
#define PATHLEN         256
#define MMAPSIZE        (1UL<<20)

int fildes;

char *addr;

void page_check(void)
{
	int i = 1;
	int flag = 0;
	int pm;
	int num_pages;
	long index;
	off_t offset;
	size_t page_sz;
	uint64_t pagemap;
	unsigned long vmstart;
	vmstart = (unsigned long)addr;
	page_sz = getpagesize();
	num_pages = MMAPSIZE / page_sz;
	index = (vmstart / page_sz) * sizeof(uint64_t);
	pm = open("/proc/self/pagemap", O_RDONLY);
	if (pm == -1) {
		if ((errno == EPERM) && (geteuid() != 0)) {
			printf("don't have permission to open dev pagemap, error number %d\n", errno);
			return;
		} else {
			printf("pen dev pagemap failed, erorr number %d\n", errno);
			exit(EXIT_FAILURE);
		}
	}
	offset = lseek(pm, index, SEEK_SET);
	if (offset != index){
		printf("Reposition offset failed, error number %d\n", errno);
		exit(EXIT_FAILURE);
	}
	while (i <= num_pages) {
		read(pm, &pagemap, sizeof(uint64_t));
		if (!(pagemap & (1ULL<<63))) {
			printf("The %dth page addressed at %lX is not present\n", i, vmstart + i * page_sz);
			flag = 1;
		}
		i++;
	}
	close(pm);
	if (!flag)
		printf("[I] All pages are present\n");
}
void verify_mmap()
{
	unsigned int i;
	addr = mmap(NULL, MMAPSIZE, PROT_READ | PROT_WRITE,
		    MAP_PRIVATE | MAP_POPULATE, fildes, 0);
	if (addr == MAP_FAILED) {
		printf("mmap of %s failed, error number %d\n", TEMPFILE, errno);
		return;
	}
	page_check();
	for (i = 0; i < MMAPSIZE; i++) {
		if (addr[i]) {
			printf("Non-zero byte at offset %i\n", i);
			munmap(addr, MMAPSIZE);
			exit(EXIT_FAILURE);
		}
	}
	printf("test succeeded\n");
	munmap(addr, MMAPSIZE);
}

void setup(void)
{
	fildes = open(TEMPFILE, O_RDWR | O_CREAT, 0766);
	ftruncate(fildes, MMAPSIZE);
}

void cleanup(void)
{
	if (fildes > 0)
		close(fildes);
}

void main(){
	setup();
	verify_mmap();
	cleanup();
}
