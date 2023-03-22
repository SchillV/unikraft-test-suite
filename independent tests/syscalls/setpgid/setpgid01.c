#include "incl.h"

void setup(void);

void cleanup(void);
char *TCID = "setpgid01";

void setpgid_test1(void);

void setpgid_test2(void);

void (*testfunc[])(void) = { setpgid_test1, setpgid_test2};
int TST_TOTAL = ARRAY_SIZE(testfunc);
int main(int ac, char **av)
{
	int i, lc;
	tst_parse_opts(ac, av, NULL, NULL);
	setup();
	for (lc = 0; TEST_LOOPING(lc); lc++) {
		tst_count = 0;
		for (i = 0; i < TST_TOTAL; i++)
			(*testfunc[i])();
	}
	cleanup();
	tst_exit();
}

void setpgid_test1(void)
{
	pid_t pgid, pid;
	pgid = getpgrp();
	pid = getpid();
setpgid(pid, pgid);
	if (TEST_RETURN == -1 || getpgrp() != pgid) {
		tst_resm(TFAIL | TTERRNO, "test setpgid(%d, %d) fail",
			 pid, pgid);
	} else {
		tst_resm(TPASS, "test setpgid(%d, %d) success", pid, pgid);
	}
}

int wait4child(pid_t child)
{
	int status;
	if (waitpid(child, &status, 0) == -1)
		tst_resm(TBROK|TERRNO, "waitpid");
	if (WIFEXITED(status))
		return WEXITSTATUS(status);
	else
		return status;
}

void setpgid_test2(void)
{
	int ret;
	pid_t pgid, pid;
	pid = FORK_OR_VFORK();
	if (pid == -1)
		tst_brkm(TBROK | TERRNO, cleanup, "fork()");
	if (pid != 0) {
		ret = wait4child(pid);
	} else {
		pid = getpid();
setpgid(0, 0);
		pgid = getpgrp();
		if (TEST_RETURN == -1) {
			fprintf(stderr, "setpgid(0, 0) fails in "
				"child process: %s\n", strerror(TEST_ERRNO));
			exit(1);
		} else if (pgid != pid) {
			fprintf(stderr, "setpgid(0, 0) fails to make PGID"
				"equal to PID\n");
			exit(1);
		} else {
			exit(0);
		}
	}
	if (ret == 0)
		tst_resm(TPASS, "test setpgid(0, 0) success");
	else
		tst_resm(TFAIL, "test setpgid(0, 0) fail");
}

void setup(void)
{
	tst_sig(FORK, DEF_HANDLER, cleanup);
	TEST_PAUSE;
}

void cleanup(void)
{
}

