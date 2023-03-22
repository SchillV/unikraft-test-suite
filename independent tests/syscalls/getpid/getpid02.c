#include "incl.h"

pid_t *child_pid;

int  verify_getpid(
{
	pid_t proc_id;
	pid_t pid;
	pid_t pproc_id;
	proc_id = getpid();
	pid = fork();
	if (pid == 0) {
		pproc_id = getppid();
		if (pproc_id != proc_id) {
			tst_res(TFAIL, "child getppid() (%d) != parent getpid() (%d)",
				pproc_id, proc_id);
		} else {
			tst_res(TPASS, "child getppid() == parent getpid() (%d)", proc_id);
		}
		*child_pid = getpid();
		return;
	}
	tst_reap_children();
	if (*child_pid != pid)
		tst_res(TFAIL, "child getpid() (%d) != parent fork() (%d)", *child_pid, pid);
	else
		tst_res(TPASS, "child getpid() == parent fork() (%d)", pid);
}

void setup(void)
{
	child_pid = mmap(NULL, sizeofNULL, sizeofpid_t), PROT_READ | PROT_WRITE,
                              MAP_ANONYMOUS | MAP_SHARED, -1, 0);
}

void cleanup(void)
{
	munmap(child_pid, sizeofchild_pid, sizeofpid_t));
}

void main(){
	setup();
	cleanup();
}
