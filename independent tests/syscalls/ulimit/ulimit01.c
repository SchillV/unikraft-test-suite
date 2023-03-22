#include "incl.h"
void setup();
void cleanup();
char *TCID = "ulimit01";
int TST_TOTAL = 6;
int cmd;
struct limits_t {
	int cmd;
	long newlimit;
	int exp_fail;
} Scenarios[] = {
	{
	UL_GETFSIZE, -1, 0, 0}, {
	{
#if UL_GMEMLIM
	{
	UL_GMEMLIM, -1, 0, 0},
#endif
#if UL_GDESLIM
	{
	UL_GDESLIM, -1, 0, 0},
#endif
#if UL_GSHMEMLIM
	{
	UL_GSHMEMLIM, -1, 0, 0},
#endif
};
int main(int ac, char **av)
{
	int lc;
	int i;
	int tmp;
	TST_TOTAL = sizeof(Scenarios) / sizeof(struct limits_t);
     * parse standard options
     ***************************************************************/
	tst_parse_opts(ac, av, NULL, NULL);
     * perform global setup for test
     ***************************************************************/
	setup();
     * check looping state if -c option given
     ***************************************************************/
	for (lc = 0; TEST_LOOPING(lc); lc++) {
		tst_count = 0;
		for (i = 0; i < TST_TOTAL; i++) {
			cmd = Scenarios[i].cmd;
			limit = Scenarios[i].newlimit;
			 * Call ulimit(2)
			 */
ulimit(cmd, limit);
			if (TEST_RETURN == -1) {
				if (Scenarios[i].exp_fail) {
					tst_resm(TPASS | TTERRNO,
						 "ulimit(%d, %ld) Failed expectedly",
						 cmd, limit);
				} else {
					tst_resm(TFAIL | TTERRNO,
						 "ulimit(%d, %ld) Failed",
						 cmd, limit);
				}
			} else {
				if (Scenarios[i].exp_fail) {
					tst_resm(TFAIL,
						 "ulimit(%d, %ld) returned %ld, succeeded unexpectedly",
						 cmd, limit, TEST_RETURN);
				} else {
					tst_resm(TPASS,
						 "ulimit(%d, %ld) returned %ld",
						 cmd, limit, TEST_RETURN);
				}
				 * Save the UL_GETFSIZE return value in the newlimit field
				 * for UL_SETFSIZE test cases.
				 */
				if (cmd == UL_GETFSIZE) {
					for (tmp = i + 1; tmp < TST_TOTAL;
					     tmp++) {
						if (Scenarios[tmp].nlim_flag ==
						    1) {
							Scenarios[tmp].newlimit
							    = TEST_RETURN;
						}
						if (Scenarios[tmp].nlim_flag ==
						    2) {
							Scenarios[tmp].newlimit
							    = TEST_RETURN - 1;
						}
					}
				}
			}
		}
	}
     * cleanup and exit
     ***************************************************************/
	cleanup();
	tst_exit();
}
void setup(void)
{
	tst_sig(NOFORK, DEF_HANDLER, cleanup);
	TEST_PAUSE;
}
void cleanup(void)
{
}

