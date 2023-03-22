#include "incl.h"

void run_child(void)
{
	const size_t msize = getpagesize();
	void *mem = NULL;
	tst_res(TINFO,
		"mmap(0, %zu, PROT_READ | PROT_WRITE, MAP_ANONYMOUS | MAP_PRIVATE, -1, 0)",
		msize);
	mem = mmap(NULL,
			msize,
			PROT_READ | PROT_WRITE,
			MAP_ANONYMOUS | MAP_PRIVATE,
			-1,
			0);
	memset(mem, 'L', msize);
	tst_res(TINFO, "madvise(%p, %zu, MADV_HWPOISON)", mem, msize);
	if (madvise(mem, msize, MADV_HWPOISON) == -1) {
		if (errno == EINVAL) {
			tst_res(TCONF | TERRNO,
				"CONFIG_MEMORY_FAILURE probably not set in kconfig");
		} else {
			tst_res(TFAIL | TERRNO, "Could not poison memory");
		}
		exit(0);
	}
	tst_res(TFAIL, "Did not receive SIGBUS on accessing poisoned page");
}

void run(void)
{
	int status;
	pid_t pid;
	pid = fork();
	if (pid == 0) {
		run_child();
		exit(0);
	}
	waitpid(pid, &status, 0);
	if (WIFSIGNALED(status) && WTERMSIG(status) == SIGBUS) {
		tst_res(TPASS, "Received SIGBUS after accessing poisoned page");
		return;
	}
	if (WIFEXITED(status) && WEXITSTATUS(status) == 0)
		return;
	tst_res(TFAIL, "Child %s", tst_strstatus(status));
}

void main(){
	setup();
	cleanup();
}
