#include "incl.h"
#define TEST_PAGES 2
#define TEST_NODES 2
#define TOUCHED_PAGES 1
#define UNTOUCHED_PAGE (TEST_PAGES - 1)
void setup(void);
void cleanup(void);
char *TCID = "move_pages04";
int TST_TOTAL = 1;
typedef void (*sighandler_t) (int);
int main(int argc, char **argv)
{
	tst_parse_opts(argc, argv, NULL, NULL);
	setup();
#ifdef HAVE_NUMA_V2
	unsigned int i;
	int lc;
	unsigned int from_node;
	unsigned int to_node;
	int ret, exp_status;
	if ((tst_kvercmp(4, 3, 0)) >= 0)
		exp_status = -EFAULT;
	else
		exp_status = -ENOENT;
	ret = get_allowed_nodes(NH_MEMS, 2, &from_node, &to_node);
	if (ret < 0)
		tst_brkm(TBROK | TERRNO, cleanup, "get_allowed_nodes: %d", ret);
	for (lc = 0; TEST_LOOPING(lc); lc++) {
		void *pages[TEST_PAGES] = { 0 };
		int nodes[TEST_PAGES];
		int status[TEST_PAGES];
		unsigned long onepage = get_page_size();
		tst_count = 0;
		ret = alloc_pages_on_node(pages, TOUCHED_PAGES, from_node);
		if (ret == -1)
			continue;
		pages[UNTOUCHED_PAGE] = numa_alloc_onnode(onepage, from_node);
		if (pages[UNTOUCHED_PAGE] == NULL) {
			tst_resm(TBROK, "failed allocating page on node %d",
				 from_node);
			goto err_free_pages;
		}
		for (i = 0; i < TEST_PAGES; i++)
			nodes[i] = to_node;
		ret = numa_move_pages(0, TEST_PAGES, pages, nodes,
				      status, MPOL_MF_MOVE);
		if (ret == -1) {
			tst_resm(TFAIL | TERRNO,
				 "move_pages unexpectedly failed");
			goto err_free_pages;
		} else if (ret > 0) {
			tst_resm(TINFO, "move_pages() returned %d", ret);
		}
		if (status[UNTOUCHED_PAGE] == exp_status) {
			tst_resm(TPASS, "status[%d] has expected value",
				 UNTOUCHED_PAGE);
		} else {
			tst_resm(TFAIL, "status[%d] is %s, expected %s",
				UNTOUCHED_PAGE,
				tst_strerrno(-status[UNTOUCHED_PAGE]),
				tst_strerrno(-exp_status));
		}
err_free_pages:
		 * untouched pages.
		 */
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
	tst_sig(FORK, DEF_HANDLER, cleanup);
	check_config(TEST_NODES);
	 * TEST_PAUSE contains the code to fork the test with the -c option.
	 */
	TEST_PAUSE;
}
void cleanup(void)
{
}

