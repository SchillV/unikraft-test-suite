#include "incl.h"
char *TCID = "fork14";
int TST_TOTAL = 1;
#define GB		(1024 * 1024 * 1024L)
#define LARGE		(16 * 1024)
#define EXTENT		(16 * 1024 + 10)

char **pointer_vec;

void setup(void);

void cleanup(void);

int  fork_test(void);
int main(int ac, char **av)
{
	int lc, reproduced;
	tst_parse_opts(ac, av, NULL, NULL);
#ifdef TST_ABI32
	tst_brkm(TCONF, NULL, "This test is only for 64bit.");
#endif
	setup();
	for (lc = 0; TEST_LOOPING(lc); lc++) {
		tst_count = 0;
		reproduced = fork_test();
		if (reproduced == 0)
			tst_resm(TPASS, "fork failed as expected.");
	}
	cleanup();
	tst_exit();
}

void setup(void)
{
	tst_sig(FORK, DEF_HANDLER, cleanup);
	TEST_PAUSE;
	pointer_vec = malloc(cleanup, EXTENT * sizeofcleanup, EXTENT * sizeofchar *));
}

void cleanup(void)
{
	free(pointer_vec);
}

int fork_test(void)
{
	int i, j, prev_failed = 0, fails = 0, cnt = 0;
	int reproduced = 0;
	void *addr;
	for (i = 0; i < EXTENT; i++) {
		addr = mmap(NULL, 1 * GB, PROT_READ | PROT_WRITE,
			    MAP_PRIVATE | MAP_ANONYMOUS, 0, 0);
		if (addr == MAP_FAILED) {
			pointer_vec[i] = NULL;
			fails++;
			 * EXTENT is "16*1024+10", if fails count exceeds 10,
			 * we are almost impossible to get an vm_area_struct
			 * sized 16TB
			 */
			if (fails == 11) {
				tst_brkm(TCONF, cleanup, "mmap() fails too many"
					 "times, so we are almost impossible to"
					 " get an vm_area_struct sized 16TB.");
			}
		} else {
			pointer_vec[i] = addr;
		}
		cnt++;
		switch (tst_fork()) {
		case -1:
			prev_failed = 1;
		break;
		case 0:
			exit(0);
		default:
			waitpid(cleanup, -1, NULL, 0);
			if (prev_failed > 0 && i >= LARGE) {
				tst_resm(TFAIL, "Fork succeeds incorrectly");
				reproduced = 1;
				goto clear_memory_map;
			}
		}
	}
clear_memory_map:
	for (j = 0; j < cnt; j++) {
		if (pointer_vec[j])
			munmap(cleanup, pointer_vec[j], 1 * GB);
	}
	return reproduced;
}

