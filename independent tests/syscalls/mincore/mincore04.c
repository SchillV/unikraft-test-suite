#include "incl.h"
#define NUM_PAGES 3

int fd;

int size;

void *ptr;

void cleanup(void)
{
	if (fd > 0)
		close(fd);
	if (ptr) {
		munlock(ptr, size);
		munmap(ptr, size);
	}
}

void setup(void)
{
	int page_size, ret;
	page_size = getpagesize();
	size = page_size * NUM_PAGES;
	fd = open("FILE", O_CREAT | O_RDWR, 0600);
	ftruncate(fd, size);
	fsync(fd);
	ret = posix_fadvise(fd, 0, size, POSIX_FADV_DONTNEED);
	if (ret == -1)
		tst_brk(TBROK | TERRNO, "fadvise failed");
}

void lock_file(void)
{
	mlock(ptr, size);
	TST_CHECKPOINT_WAKE(0);
	TST_CHECKPOINT_WAIT(1);
}

int count_pages_in_cache(void)
{
	int locked_pages = 0;
	int count, ret;
	unsigned char vec[NUM_PAGES];
	TST_CHECKPOINT_WAIT(0);
	ret = mincore(ptr, size, vec);
	if (ret == -1)
		tst_brk(TBROK | TERRNO, "mincore failed");
	for (count = 0; count < NUM_PAGES; count++) {
		if (vec[count] & 1)
			locked_pages++;
	}
	TST_CHECKPOINT_WAKE(1);
	return locked_pages;
}

void test_mincore(void)
{
	int  locked_pages;
	ptr = mmap(NULL, size, PROT_READ | PROT_WRITE, MAP_PRIVATE, fd, 0);
	pid_t child_pid = fork();
	if (child_pid == 0) {
		lock_file();
		exit(0);
	}
	locked_pages = count_pages_in_cache();
	tst_reap_children();
	if (locked_pages == NUM_PAGES)
		tst_res(TPASS, "mincore reports all %d pages locked by child process "
			"are resident", locked_pages);
	else
		tst_res(TFAIL, "mincore reports %d pages resident but %d pages "
			"locked by child process", locked_pages, NUM_PAGES);
}

void main(){
	setup();
	cleanup();
}
