#include "incl.h"
#define TEST_PAGES 2
#define TEST_NODES TEST_PAGES
void setup(void);
void cleanup(void);
char *TCID = "move_pages01";
int TST_TOTAL = 1;
int main(int argc, char **argv)
{
	tst_parse_opts(argc, argv, NULL, NULL);
	setup();
#ifdef HAVE_NUMA_V2
	int lc;
	for (lc = 0; TEST_LOOPING(lc); lc++) {
		void *pages[TEST_PAGES] = { 0 };
		int status[TEST_PAGES];
		int ret;
		tst_count = 0;
		ret = alloc_pages_linear(pages, TEST_PAGES);
		if (ret == -1)
			continue;
		ret = numa_move_pages(0, TEST_PAGES, pages, NULL, status, 0);
		if (ret < 0) {
			tst_resm(TFAIL|TERRNO, "move_pages failed");
			free_pages(pages, TEST_PAGES);
			continue;
		} else if (ret > 0) {
			tst_resm(TINFO, "move_pages() returned %d", ret);
		}
int 		verify_pages_linear(pages, status, TEST_PAGES);
		free_pages(pages, TEST_PAGES);
	}
#else
	tst_resm(TCONF, NUMA_ERROR_MSG);
#endif
	cleanup();
	tst_exit();
}
void setup(void)
{
	tst_sig(NOFORK, DEF_HANDLER, cleanup);
	check_config(TEST_NODES);
	 * TEST_PAUSE contains the code to fork the test with the -c option.
	 */
	TEST_PAUSE;
}
void cleanup(void)
{
}

