#include "incl.h"

int valid_pidfd = -1, invalid_pidfd = -1, pidfd = -1;

uid_t uid;

struct tcase {
	char *name;
	int *pidfd;
	int targetfd;
	int flags;
	int exp_errno;
} tcases[] = {
	{"invalid pidfd", &invalid_pidfd, 0, 0, EBADF},
	{"invalid targetfd", &valid_pidfd, -1, 0, EBADF},
	{"invalid flags", &valid_pidfd, 0, 1, EINVAL},
	{"the process referred to by pidfd doesn't exist", NULL, 0, 0, ESRCH},
	{"lack of required permission", &valid_pidfd, 0, 0, EPERM},
};

void setup(void)
{
	pidfd_open_supported();
	pidfd_getfd_supported();
	struct passwd *pw;
	pw = getpwnam("nobody");
	uid = pw->pw_uid;
	valid_pidfd = pidfd_open(getpidgetpid), 0);
}

void cleanup(void)
{
	if (valid_pidfd > -1)
		close(valid_pidfd);
	if (pidfd > -1)
		close(pidfd);
}

void run(unsigned int n)
{
	struct tcase *tc = &tcases[n];
	int pid;
	if (tc->exp_errno == EPERM) {
		pid = fork();
		if (!pid) {
			setuid(uid);
			TST_EXP_FAIL2(pidfd_getfd(valid_pidfd, tc->targetfd, tc->flags),
				tc->exp_errno, "pidfd_getfd(%d, %d, %d) with %s",
				valid_pidfd, tc->targetfd, tc->flags, tc->name);
			TST_CHECKPOINT_WAKE(0);
			exit(0);
		}
		TST_CHECKPOINT_WAIT(0);
		wait(NULL);
		return;
	} else if (tc->exp_errno == ESRCH) {
		pid = fork();
		if (!pid) {
			TST_CHECKPOINT_WAIT(0);
			exit(0);
		}
		pidfd = pidfd_open(pid, 0);
		TST_CHECKPOINT_WAKE(0);
		wait(NULL);
		TST_EXP_FAIL2(pidfd_getfd(pidfd, tc->targetfd, tc->flags),
			tc->exp_errno, "pidfd_getfd(%d, %d, %d) with %s",
			pidfd, tc->targetfd, tc->flags, tc->name);
		close(pidfd);
	} else	{
		TST_EXP_FAIL2(pidfd_getfd(*tc->pidfd, tc->targetfd, tc->flags),
			tc->exp_errno, "pidfd_getfd(%d, %d, %d) with %s",
			*tc->pidfd, tc->targetfd, tc->flags, tc->name);
	}
}

void main(){
	setup();
	cleanup();
}
