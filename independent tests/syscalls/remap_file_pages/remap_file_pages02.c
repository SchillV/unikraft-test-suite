#include "incl.h"
#define _GNU_SOURCE
#define WINDOW_START 0x48000000

unsigned int page_sz;

int fd;

char *data = NULL;

char *data01 = NULL;

void setup01(int test);

void setup02(int test);

void setup03(int test);

void setup04(int test);

struct tcase {
	char *err_desc;
	int exp_errno;
	void (*setup)(int);
	void *start;
	size_t size;
	int prot;
	ssize_t pgoff;
	int flags;
} tcases[] = {
	{"start is not valid MAP_SHARED mapping",
	 EINVAL, setup01, NULL, 0, 0, 2, 0},
	{"start is invalid", EINVAL, setup02, NULL, 0, 0, 2, 0},
	{"size is invalid", EINVAL, setup03,  NULL, 0, 0, 0, 0},
	{"prot is invalid", EINVAL, setup04, NULL, 0, 0, 2, 0}
};

void run(unsigned i)
{
tst_syscall(__NR_remap_file_page;
			 tcases[i].start, tcases[i].size,
			 tcases[i].prot, tcases[i].pgoff,
			 tcases[i].flags));
	if ((TST_RET == -1) && (TST_ERR == tcases[i].exp_errno)) {
		tst_res(TPASS | TTERRNO, "remap_file_pages(2) %s",
			tcases[i].err_desc);
		return;
	}
	tst_res(TFAIL | TTERRNO,
		"remap_file_pages(2) %s expected %s got",
		tcases[i].err_desc, tst_strerrno(tcases[i].exp_errno));
}

void setup01(int test)
{
	tcases[test].start = data01;
	tcases[test].size = page_sz;
}

void setup02(int test)
{
	tcases[test].start = data + page_sz;
	tcases[test].size = page_sz;
}

void setup03(int test)
{
	tcases[test].start = data;
	tcases[test].size = 2 * page_sz;
}

void setup04(int test)
{
	tcases[test].start = data;
	tcases[test].size = page_sz;
	tcases[test].prot = -1;
}

void setup(void)
{
	unsigned int i;
	page_sz = getpagesize();
	fd = open("cache", O_RDWR | O_CREAT | O_TRUNC, S_IRWXU);
	ftruncate(fd, page_sz);
	data = mmap(void *)WINDOW_START, page_sz, PROT_READ | PROT_WRITE,
			 MAP_SHARED, fd, 0);
	data01 = mmap(NULL, page_sz, PROT_READ | PROT_WRITE, MAP_PRIVATE,
			   fd, 0);
	for (i = 0; i < ARRAY_SIZE(tcases); i++) {
		if (tcases[i].setup)
			tcases[i].setup(i);
	}
}

void cleanup(void)
{
	close(fd);
	if (data)
		munmap(data, page_sz);
	if (data01)
		munmap(data01, page_sz);
}

void main(){
	setup();
	cleanup();
}
