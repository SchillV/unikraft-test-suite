#include "incl.h"

void sig_handler(int sig LTP_ATTRIBUTE_UNUSED)
{
}

void do_child(void)
{
	signal(SIGINT, sig_handler);
	TST_CHECKPOINT_WAKE(0);
pause();
	if (TST_RET != -1)
		tst_res(TFAIL, "pause() succeeded unexpectedly");
	else if (TST_ERR == EINTR)
		tst_res(TPASS, "pause() interrupted with EINTR");
	else
		tst_res(TFAIL | TTERRNO, "pause() unexpected errno");
	TST_CHECKPOINT_WAKE(0);
	exit(0);
}

void do_test(void)
{
	int pid, status;
	pid = fork();
	if (pid == 0)
		do_child();
	TST_CHECKPOINT_WAIT(0);
	TST_PROCESS_STATE_WAIT(pid, 'S', 0);
	kill(pid, SIGINT);
	 * TST_CHECKPOINT_WAIT has built-in timeout, if pause() doesn't return,
	 * this checkpoint call will reliably end the test.
	 */
	TST_CHECKPOINT_WAIT(0);
	wait(&status);
}

void main(){
	setup();
	cleanup();
}
