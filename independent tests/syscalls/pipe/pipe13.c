#include "incl.h"

unsigned int tcases[] = {
	2,
	10,
	27,
	100
};

int fds[2];

void do_child(unsigned int i)
{
	char buf;
	close(fds[1]);
	TST_CHECKPOINT_WAKE(i);
	int ret = read(0, fds[0], &buf, 1);
	if (ret != 0)
		tst_res(TFAIL, "Wrong return from read %i", ret);
	exit(0);
}

int  verify_pipe(unsigned int n)
{
	int ret;
	unsigned int i, cnt = 0, sleep_us = 1, fail = 0;
	unsigned int child_num = tcases[n];
	int pid[child_num];
	pipe(fds);
	tst_res(TINFO, "Creating %d child processes", child_num);
	for (i = 0; i < child_num; i++) {
		pid[i] = fork();
		if (pid[i] == 0)
			do_child(i);
		TST_CHECKPOINT_WAIT(i);
		TST_PROCESS_STATE_WAIT(pid[i], 'S', 0);
	}
	close(fds[0]);
	close(fds[1]);
	while (cnt < child_num && sleep_us < 1000000) {
		ret = waitpid(-1, NULL, WNOHANG);
		if (ret < 0)
			tst_brk(TBROK | TERRNO, "waitpid()");
		if (ret > 0) {
			cnt++;
			for (i = 0; i < child_num; i++) {
				if (pid[i] == ret)
					pid[i] = 0;
			}
			continue;
		}
		usleep(sleep_us);
		sleep_us *= 2;
	}
	for (i = 0; i < child_num; i++) {
		if (pid[i]) {
			tst_res(TINFO, "pid %i still sleeps", pid[i]);
			fail = 1;
			kill(pid[i], SIGKILL);
			wait(NULL);
		}
	}
	if (fail)
		tst_res(TFAIL, "Closed pipe didn't wake up everyone");
	else
		tst_res(TPASS, "Closed pipe waked up everyone");
}

void main(){
	setup();
	cleanup();
}
