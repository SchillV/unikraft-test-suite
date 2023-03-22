#include "incl.h"

pid_t pgid, pid, ppid, init_pgid;

pid_t negative_pid = -1;

struct tcase {
	pid_t *pid;
	pid_t *pgid;
	int error;
} tcases[] = {
	{&pid, &negative_pid, EINVAL},
	{&ppid, &pgid, ESRCH},
	{&pid, &init_pgid, EPERM}
};

void setup(void)
{
	pid = getpid();
	ppid = getppid();
	pgid = getpgrp();
	 * Getting pgid of init/systemd process to use it as a
	 * process group from a different session for EPERM test
	 */
	init_pgid = getpgid(1);
}

void run(unsigned int n)
{
	struct tcase *tc = &tcases[n];
	TST_EXP_FAIL(setpgid(*tc->pid, *tc->pgid), tc->error,
				"setpgid(%d, %d)", *tc->pid, *tc->pgid);
}

void main(){
	setup();
	cleanup();
}
