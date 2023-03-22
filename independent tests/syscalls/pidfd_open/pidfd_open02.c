#include "incl.h"

pid_t expired_pid, my_pid, invalid_pid = -1;

struct tcase {
	char *name;
	pid_t *pid;
	int flags;
	int exp_errno;
} tcases[] = {
	{"expired pid", &expired_pid, 0, ESRCH},
	{"invalid pid", &invalid_pid, 0, EINVAL},
	{"invalid flags", &my_pid, 1, EINVAL},
};

void setup(void)
{
	pidfd_open_supported();
	expired_pid = tst_get_unused_pid();
	my_pid = getpid();
}

void run(unsigned int n)
{
	struct tcase *tc = &tcases[n];
	TST_EXP_FAIL2(pidfd_open(*tc->pid, tc->flags), tc->exp_errno,
			"pidfd_open with %s", tc->name);
}

void main(){
	setup();
	cleanup();
}
