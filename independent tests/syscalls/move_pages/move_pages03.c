#include "incl.h"
#define TEST_PAGES 2
#define TEST_NODES 2
enum {
	SEM_CHILD_SETUP,
	SEM_PARENT_TEST,
	MAX_SEMS
};
void setup(void);
void cleanup(void);
char *TCID = "move_pages03";
int TST_TOTAL = 1;
void child(void **pages, sem_t * sem)
{
	int i;
	for (i = 0; i < TEST_PAGES; i++) {
		char *page;
		page = pages[i];
		page[0] = 0xAA;
	}
	if (sem_post(&sem[SEM_CHILD_SETUP]) == -1)
		tst_resm(TWARN | TERRNO, "error post semaphore");
	if (sem_wait(&sem[SEM_PARENT_TEST]) == -1)
		tst_resm(TWARN | TERRNO, "error wait semaphore");
	exit(0);
}
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
		pid_t cpid;
		sem_t *sem;
		tst_count = 0;
		ret = alloc_shared_pages_on_node(pages, TEST_PAGES, from_node);
		if (ret == -1)
			continue;
		for (i = 0; i < TEST_PAGES; i++) {
			nodes[i] = to_node;
		}
		sem = alloc_sem(MAX_SEMS);
		if (sem == NULL) {
			goto err_free_pages;
		}
		 * Fork a child process so that the shared pages are
		 * now really shared between two processes.
		 */
		cpid = fork();
		if (cpid == -1) {
			tst_resm(TBROK | TERRNO, "forking child failed");
			goto err_free_sem;
		} else if (cpid == 0) {
			child(pages, sem);
		}
		if (sem_wait(&sem[SEM_CHILD_SETUP]) == -1)
			tst_resm(TWARN | TERRNO, "error wait semaphore");
		ret = numa_move_pages(0, TEST_PAGES, pages, nodes,
				      status, MPOL_MF_MOVE_ALL);
		if (ret < 0) {
			tst_resm(TFAIL|TERRNO, "move_pages failed");
			goto err_kill_child;
		} else if (ret > 0) {
			tst_resm(TINFO, "move_pages() returned %d", ret);
		}
int 		verify_pages_on_node(pages, status, TEST_PAGES, to_node);
err_kill_child:
		if (sem_post(&sem[SEM_PARENT_TEST]) == -1)
			tst_resm(TWARN | TERRNO, "error post semaphore");
		wait(NULL);
err_free_sem:
		free_sem(sem, MAX_SEMS);
err_free_pages:
		free_shared_pages(pages, TEST_PAGES);
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

