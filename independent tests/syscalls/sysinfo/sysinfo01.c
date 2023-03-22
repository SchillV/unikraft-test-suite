#include "incl.h"
void setup();
void cleanup();
char *TCID = "sysinfo01";
int TST_TOTAL = 1;
int main(int ac, char **av)
{
	struct sysinfo *sys_buf;
	int lc;
	float l1, l2, l3;
	unsigned long l1_up, l2_up, l3_up;
	sys_buf = malloc(sizeof(struct sysinfo));
	tst_parse_opts(ac, av, NULL, NULL);
	for (lc = 0; TEST_LOOPING(lc); lc++) {
		tst_count = 0;
sysinfo(sys_buf);
		if (TEST_RETURN == -1) {
			tst_brkm(TFAIL, cleanup, "sysinfo() Failed, errno=%d"
				 " : %s", TEST_ERRNO, strerror(TEST_ERRNO));
		} else {
			 * used by sysinfo03 to test the functionality of
			 * sysinfo.
			 */
			if (ac == 2 && !strncmp(av[1], "TEST3", 5)) {
				tst_resm(TINFO, "Generating info for "
					 "sysinfo03");
				l1 = sys_buf->loads[0] / 60000.0;
				l2 = sys_buf->loads[1] / 60000.0;
				l3 = sys_buf->loads[2] / 60000.0;
				l1_up = l1 * 100;
				l2_up = l2 * 100;
				l3_up = l3 * 100;
				sys_buf->loads[0] = sys_buf->loads[0] / 10;
				sys_buf->loads[1] = sys_buf->loads[1] / 10;
				sys_buf->loads[2] = sys_buf->loads[2] / 10;
				printf("uptime %lu\n", sys_buf->uptime);
				printf("load1 %lu\n", sys_buf->loads[0]);
				printf("load2 %lu\n", sys_buf->loads[1]);
				printf("load3 %lu\n", sys_buf->loads[2]);
				printf("l1 %lu\n", l1_up);
				printf("l2 %lu\n", l2_up);
				printf("l3 %lu\n", l3_up);
				printf("totalram %lu\n", sys_buf->totalram);
				printf("freeram  %lu\n", sys_buf->freeram);
				printf("sharedram %lu\n", sys_buf->sharedram);
				printf("bufferram %lu\n", sys_buf->bufferram);
				printf("totalswap %lu\n",
				       sys_buf->totalswap / (1024 * 1024));
				printf("freeswap %lu\n", sys_buf->freeswap);
				printf("procs %lu\n",
				       (unsigned long)sys_buf->procs);
			} else {
				tst_resm(TPASS,
					 "Test to check the return code PASSED");
			}
		}
	}
	cleanup();
	tst_exit();
}
void setup(void)
{
	tst_sig(FORK, DEF_HANDLER, cleanup);
	umask(0);
	TEST_PAUSE;
}
void cleanup(void)
{
}

