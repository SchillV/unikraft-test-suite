#include "incl.h"
#define TEST_APP "creat07_child"

int  verify_creat(
{
	pid_t pid;
	pid = fork();
	if (pid == 0) {
		execl(TEST_APP, TEST_APP, NULL);
		exit(1);
	}
	TST_CHECKPOINT_WAIT(0);
creat(TEST_APP, O_WRONLY);
	if (TST_RET != -1) {
		tst_res(TFAIL, "creat() succeeded unexpectedly");
		return;
	}
	if (TST_ERR == ETXTBSY)
		tst_res(TPASS, "creat() received EXTBSY");
	else
		tst_res(TFAIL | TTERRNO, "creat() failed unexpectedly");
	kill(pid, SIGKILL);
	waitpid(pid, NULL, 0);
}

void main(){
	setup();
	cleanup();
}
