#include "incl.h"
#define _GNU_SOURCE 1

void cleanup_pid(pid_t pid)
{
	if (pid > 0) {
		kill(pid, SIGKILL);
		waitpid(pid, NULL, 0);
	}
}

void case0(void)
{
	pid_t pid, ret;
	int status;
	pid = fork();
	if (pid == 0) {
		TST_CHECKPOINT_WAIT(0);
		exit(0);
	}
	for (;;) {
		ret = waitpid(pid, &status, WNOHANG);
		if ((ret == -1) && (errno == EINTR))
			continue;
		if (ret == 0)
			break;
		tst_res(TFAIL, "waitpid(WNOHANG) returned %d, expected 0",
			ret);
		cleanup_pid(pid);
		return;
	}
	TST_CHECKPOINT_WAKE(0);
	waitpid(pid, NULL, 0);
	tst_res(TPASS, "waitpid(pid, WNOHANG) = 0 for a running child");
}

void case1(void)
{
	pid_t pid, ret;
	int status;
	pid = fork();
	if (pid == 0)
		exit(0);
	for (;;) {
		ret = waitpid(pid, &status, WNOHANG);
		if ((ret == -1) && (errno == EINTR))
			continue;
		if (ret == 0)
			continue;
		if (ret == pid)
			break;
		tst_res(TFAIL, "waitpid(WNOHANG) returned %d, expected %d",
			ret, pid);
		cleanup_pid(pid);
		return;
	}
	if (!WIFEXITED(status)) {
		tst_res(TFAIL, "Child exited abnormally");
		return;
	}
	if (WEXITSTATUS(status) != 0) {
		tst_res(TFAIL, "Child exited with %d, expected 0",
			WEXITSTATUS(status));
		return;
	}
	tst_res(TPASS, "waitpid(pid, WNOHANG) = pid for an exited child");
}

void case2(void)
{
	pid_t ret;
	int status;
	ret = waitpid(-1, &status, 0);
	if (ret != -1) {
		tst_res(TFAIL, "Expected -1, got %d", ret);
		return;
	}
	if (errno != ECHILD) {
		tst_res(TFAIL, "Expected %s, got %s",
			tst_strerrno(ECHILD), tst_strerrno(errno));
		return;
	}
	tst_res(TPASS, "waitpid(-1, 0) = -1 with ECHILD if no children");
}

void case3(void)
{
	pid_t ret;
	int status;
	ret = waitpid(-1, &status, WNOHANG);
	if (ret != -1) {
		tst_res(TFAIL, "WNOHANG: Expected -1, got %d", ret);
		return;
	}
	if (errno != ECHILD) {
		tst_res(TFAIL, "WNOHANG: Expected %s, got %s",
			tst_strerrno(ECHILD), tst_strerrno(errno));
		return;
	}
	tst_res(TPASS, "waitpid(-1, WNOHANG) = -1 with ECHILD if no children");
}

void (*tests[])(void) = { case0, case1, case2, case3 };

void waitpid09_test(unsigned int id)
{
	tests[id]();
}

void main(){
	setup();
	cleanup();
}
