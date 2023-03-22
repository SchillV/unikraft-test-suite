#include "incl.h"

void setup(void);

void cleanup(void);
char *TCID = "waitpid04";
int TST_TOTAL = 1;
#define INVAL_FLAG	-1

int flag, condition_number;
int main(int ac, char **av)
{
	int pid, status, ret;
	int lc;
	tst_parse_opts(ac, av, NULL, NULL);
	setup();
	for (lc = 0; TEST_LOOPING(lc); lc++) {
		tst_count = 0;
		ret = waitpid(pid, &status, WNOHANG);
		flag = 0;
		condition_number = 1;
		if (ret != -1) {
			tst_resm(TFAIL, "condition %d test failed",
				 condition_number);
		} else {
			if (errno != ECHILD) {
				tst_resm(TFAIL, "waitpid() set invalid "
					 "errno, expected ECHILD, got: %d",
					 errno);
			} else {
				tst_resm(TPASS, "condition %d test passed",
					 condition_number);
			}
		}
		condition_number++;
		if (FORK_OR_VFORK() == 0)
			exit(0);
		pid = 1;
		ret = waitpid(pid, &status, WUNTRACED);
		flag = 0;
		if (ret != -1) {
			tst_resm(TFAIL, "condition %d test failed",
				 condition_number);
		} else {
			if (errno != ECHILD) {
				tst_resm(TFAIL, "waitpid() set invalid "
					 "errno, expected ECHILD, got: %d",
					 errno);
			} else {
				tst_resm(TPASS, "condition %d test passed",
					 condition_number);
			}
		}
		condition_number++;
		ret = waitpid(pid, &status, INVAL_FLAG);
		flag = 0;
		if (ret != -1) {
			tst_resm(TFAIL, "condition %d test failed",
				 condition_number);
		} else {
			if (errno != EINVAL) {
				tst_resm(TFAIL, "waitpid() set invalid "
					 "errno, expected EINVAL, got: %d",
					 errno);
			} else {
				tst_resm(TPASS, "condition %d test passed",
					 condition_number);
			}
		}
		condition_number++;
	}
	cleanup();
	tst_exit();
}

void setup(void)
{
	TEST_PAUSE;
}

void cleanup(void)
{
}

