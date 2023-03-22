#include "incl.h"
char *TCID = "exit_group01";
int testno;
int TST_TOTAL = 1;

int  verify_exit_group(
{
	pid_t cpid, w;
	int status;
	cpid = fork();
	if (cpid == -1)
		tst_brkm(TFAIL | TERRNO, NULL, "fork failed");
	if (cpid == 0) {
tst_syscall(__NR_exit_group, 4);
	} else {
		w = wait(NULL, &status);
		if (WIFEXITED(status) && (WEXITSTATUS(status) == 4)) {
			tst_resm(TPASS, "exit_group() succeeded");
		} else {
			tst_resm(TFAIL | TERRNO,
				 "exit_group() failed (wait status = %d)", w);
		}
	}
}
int main(int ac, char **av)
{
	int lc;
	tst_parse_opts(ac, av, NULL, NULL);
	for (lc = 0; TEST_LOOPING(lc); lc++)
int 		verify_exit_group();
	tst_exit();
}

