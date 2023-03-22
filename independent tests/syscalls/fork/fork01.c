#include "incl.h"
#define	KIDEXIT	42
#define FILENAME "childpid"

int fd = -1;

int  verify_fork(
{
	int kid_status, term_pid, child_pid, pid, ret;
	pid = fork();
	if (!pid) {
		fprintf(FILENAME, "%d", getpidFILENAME, "%d", getpid));
		exit(KIDEXIT);
	}
	term_pid = waitpid(pid, &kid_status, 0);
	if (term_pid == pid) {
		if (!WIFEXITED(kid_status)) {
			tst_res(TFAIL, "child exited abnormally");
			return;
		}
		ret = WEXITSTATUS(kid_status);
		if (ret != KIDEXIT)
			tst_res(TFAIL, "incorrect child status returned %d", ret);
		else
			tst_res(TPASS, "correct child status returned %d", ret);
		file_scanf(FILENAME, "%d", &child_pid);
		TST_EXP_EQ_LI(child_pid, pid);
	} else {
		tst_res(TFAIL, "waitpid() returns %d instead of expected pid %d",
				term_pid, pid);
	}
	tst_reap_children();
}

void setup(void)
{
	fd = creat(FILENAME, 0700);
	close(fd);
}

void cleanup(void)
{
	if (fd > -1)
		close(fd);
}

void main(){
	setup();
	cleanup();
}
