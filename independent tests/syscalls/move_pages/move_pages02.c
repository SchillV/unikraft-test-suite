#include "incl.h"
#define TEST_PAGES 2
#define TEST_NODES 2
void setup(void);
void cleanup(void);
char *TCID = "move_pages02";
int TST_TOTAL = 1;
int main(int argc, char **argv)
{
	tst_parse_opts(argc, argv, NULL, NULL);
	setup();
#ifdef HAVE_NUMA_V2
	unsigned int i;
	int lc;
	unsigned int from_node;
	unsigned int to_node;
	int ret;
	ret = get_allowed_nodes(NH_MEMS, 2, &from_node, &to_node);
	if (ret < 0)
		tst_brkm(TBROK | TERRNO, cleanup, "get_allowed_nodes: %d", ret);
	for (lc = 0; TEST_LOOPING(lc); lc++) {
		void *pages[TEST_PAGES] = { 0 };
		int nodes[TEST_PAGES];
		int status[TEST_PAGES];
		tst_count = 0;
		ret = alloc_pages_on_node(pages, TEST_PAGES, from_node);
		if (ret == -1)
			continue;
		for (i = 0; i < TEST_PAGES; i++)
			nodes[i] = to_node;
		ret =
		    numa_move_pages(0, TEST_PAGES, pages, nodes, status,
				    MPOL_MF_MOVE);
		if (ret < 0) {
			tst_resm(TFAIL|TERRNO, "move_pages failed");
			free_pages(pages, TEST_PAGES);
			continue;
		} else if (ret > 0) {
			tst_resm(TINFO, "move_pages() returned %d", ret);
		}
		for (i = 0; i < TEST_PAGES; i++)
			*((char *)pages[i]) = 0xAA;
int 		verify_pages_on_node(pages, status, TEST_PAGES, to_node);
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

