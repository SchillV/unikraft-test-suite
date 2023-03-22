#include "incl.h"
#define TEMPFILE        "mmapfile"
#define MMAPSIZE        (1UL<<20)
#define LINELEN         256
char *TCID = "mmap14";
int TST_TOTAL = 1;

char *addr;

void getvmlck(unsigned int *lock_sz);

void setup(void);

void cleanup(void);
int main(int argc, char *argv[])
{
	int lc;
	unsigned int sz_before;
	unsigned int sz_after;
	unsigned int sz_ch;
	setup();
	getvmlck(&sz_before);
	addr = mmap(NULL, MMAPSIZE, PROT_READ | PROT_WRITE, MAP_PRIVATE | MAP_LOCKED | MAP_ANONYMOUS,-1, 0);
	if (addr == MAP_FAILED) {
		printf("mmap of %s failed, error number %d\n", TEMPFILE, errno);
	}
	getvmlck(&sz_after);
	sz_ch = sz_after - sz_before;
	if (sz_ch == MMAPSIZE / 1024) {
			printf("test succeeded\n");
	} else {
		printf("Expected %luK locked, get %uK locked", MMAPSIZE / 1024, sz_ch);
	}
	if (munmap(addr, MMAPSIZE) != 0){
		printf("munmap failed, erorr number %d\n", errno);
	}
	cleanup();
}
void getvmlck(unsigned int *lock_sz)
{
	int ret;
	char line[LINELEN];
	FILE *fstatus = NULL;
	fstatus = fopen("/proc/self/status", "r");
	if (fstatus == NULL)
		printf("Open dev status failed, error number %d\n", errno);
	while (fgets(line, LINELEN, fstatus) != NULL)
		if (strstr(line, "VmLck") != NULL)
			break;
	ret = sscanf(line, "%*[^0-9]%d%*[^0-9]", lock_sz);
	if (ret != 1)
		printf("Get lock size failed, error number %d\n", errno);
	fclose(fstatus);
}

void setup(void)
{
}

void cleanup(void)
{
}

