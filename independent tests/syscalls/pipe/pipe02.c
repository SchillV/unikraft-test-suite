#include "incl.h"
#define SIZE	5

int fd[2];

char rdbuf[SIZE];

char wrbuf[SIZE];

void do_child(void)
{
	signal(SIGPIPE, SIG_DFL);
	close(fd[0]);
	write(1, fd[1], wrbuf, SIZE);
	TST_CHECKPOINT_WAIT(0);
	write(1, fd[1], wrbuf, SIZE);
	exit(0);
}

int  verify_pipe(
{
	int status;
	int sig = 0;
	pid_t pid;
	memset(wrbuf, 'a', SIZE);
#ifdef UCLINUX
	maybe_run_child(&do_child, "dd", &fd[0], &fd[1]);
#endif
pipe(fd);
	if (TST_RET == -1) {
		tst_res(TFAIL|TTERRNO, "pipe() failed");
		return;
	}
	pid = fork();
	if (pid == 0) {
#ifdef UCLINUX
		if (self_exec(av[0], "dd", fd[0], fd[1]) < 0)
			tst_brk(TBROK, "self_exec failed");
#else
		do_child();
#endif
	}
	memset(rdbuf, 0, SIZE);
	close(fd[1]);
	read(1, fd[0], rdbuf, SIZE);
	if (memcmp(wrbuf, rdbuf, SIZE) != 0) {
		tst_res(TFAIL, "pipe read data and pipe "
			"write data didn't match");
		return;
	}
	close(fd[0]);
	TST_CHECKPOINT_WAKE(0);
	wait(&status);
	if (!WIFSIGNALED(status)) {
		tst_res(TFAIL, "Child wasn't killed by signal");
	} else {
		sig = WTERMSIG(status);
		if (sig != SIGPIPE) {
			tst_res(TFAIL, "Child killed by %s expected SIGPIPE",
				tst_strsig(sig));
		} else {
				tst_res(TPASS, "Child killed by SIGPIPE");
		}
	}
}

void main(){
	setup();
	cleanup();
}
