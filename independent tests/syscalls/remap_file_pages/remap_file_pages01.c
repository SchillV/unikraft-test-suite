#include "incl.h"
#define _GNU_SOURCE
#define WINDOW_START 0x48000000

int page_sz;
size_t page_words;
size_t cache_pages;
size_t cache_sz;
size_t window_pages;
size_t window_sz;

void setup();

void cleanup();

void test_nonlinear(int fd);
char *TCID = "remap_file_pages01";
int TST_TOTAL = 2;

char *cache_contents;
char fname[255];
int main(int ac, char **av)
{
	int lc;
	tst_parse_opts(ac, av, NULL, NULL);
	setup();
	for (lc = 0; TEST_LOOPING(lc); lc++) {
		tst_count = 0;
		test_nonlinear(fd1);
		tst_resm(TPASS, "Non-Linear shm file OK");
		test_nonlinear(fd2);
		tst_resm(TPASS, "Non-Linear /tmp/ file OK");
	}
	cleanup();
	tst_exit();
}

void test_nonlinear(int fd)
{
	char *data = NULL;
	int i, j, repeat = 2;
	for (i = 0; i < (int)cache_pages; i++) {
		char *page = cache_contents + i * page_sz;
		for (j = 0; j < (int)page_words; j++)
			page[j] = i;
	}
	if (write(fd, cache_contents, cache_sz) != (int)cache_sz) {
		tst_resm(TFAIL,
			 "Write Error for \"cache_contents\" to \"cache_sz\" of %zu (errno=%d : %s)",
			 cache_sz, errno, strerror(errno));
		cleanup(NULL);
	}
	data = mmap((void *)WINDOW_START,
		    window_sz, PROT_READ | PROT_WRITE, MAP_SHARED, fd, 0);
	if (data == MAP_FAILED) {
		tst_resm(TFAIL, "mmap Error, errno=%d : %s", errno,
			 strerror(errno));
		cleanup(NULL);
	}
again:
	for (i = 0; i < (int)window_pages; i += 2) {
		char *page = data + i * page_sz;
		if (remap_file_pages(page, page_sz * 2, 0,
				     (window_pages - i - 2), 0) == -1) {
			tst_resm(TFAIL | TERRNO,
				 "remap_file_pages error for page=%p, "
				 "page_sz=%d, window_pages=%zu",
				 page, (page_sz * 2), (window_pages - i - 2));
			cleanup(data);
		}
	}
	for (i = 0; i < (int)window_pages; i++) {
		 * Double-check the correctness of the mapping:
		 */
		if (i & 1) {
			if (data[i * page_sz] != ((int)window_pages) - i) {
				tst_resm(TFAIL,
					 "hm, mapped incorrect data, "
					 "data[%d]=%d, (window_pages-%d)=%zu",
					 (i * page_sz), data[i * page_sz], i,
					 (window_pages - i));
				cleanup(data);
			}
		} else {
			if (data[i * page_sz] != ((int)window_pages) - i - 2) {
				tst_resm(TFAIL,
					 "hm, mapped incorrect data, "
					 "data[%d]=%d, (window_pages-%d-2)=%zu",
					 (i * page_sz), data[i * page_sz], i,
					 (window_pages - i - 2));
				cleanup(data);
			}
		}
	}
	if (--repeat)
		goto again;
	munmap(data, window_sz);
}
void setup(void)
{
	tst_sig(FORK, DEF_HANDLER, cleanup);
	tst_tmpdir();
	TEST_PAUSE;
	page_sz = getpagesize();
	page_words = page_sz;
	cache_pages = 1024;
	cache_sz = cache_pages * page_sz;
	cache_contents = malloc(cache_sz * sizeof(char));
	window_pages = 16;
	window_sz = window_pages * page_sz;
	sprintf(fname, "/dev/shm/cache_%d", getpid());
	if ((fd1 = open(fname, O_RDWR | O_CREAT | O_TRUNC, S_IRWXU)) < 0) {
		tst_brkm(TBROK, cleanup,
			 "open(%s, O_RDWR|O_CREAT|O_TRUNC,S_IRWXU) Failed, errno=%d : %s",
			 fname, errno, strerror(errno));
	}
	if ((fd2 = open("cache", O_RDWR | O_CREAT | O_TRUNC, S_IRWXU)) < 0) {
		tst_brkm(TBROK, cleanup,
			 "open(%s, O_RDWR|O_CREAT|O_TRUNC,S_IRWXU) Failed, errno=%d : %s",
			 "cache", errno, strerror(errno));
	}
}
* cleanup() - Performs one time cleanup for this test at
* completion or premature exit
*/
void cleanup(char *data)
{
	close(fd1);
	close(fd2);
	if (data)
		munmap(data, window_sz);
	unlink(fname);
	tst_rmdir();
}

