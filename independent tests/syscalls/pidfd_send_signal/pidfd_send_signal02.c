#include "incl.h"
#define _GNU_SOURCE
#define CORRECT_SIGNAL		SIGUSR1
#define DIFFERENT_SIGNAL	SIGUSR2

siginfo_t info;

int pidfd;

int init_pidfd;

int dummyfd;

struct tcase {
	int		*fd;
	siginfo_t	*siginf;
	int		signal;
	int		flags;
	int		exp_err;
} tcases[] = {
	{&pidfd, &info, CORRECT_SIGNAL, 99999, EINVAL},
	{&dummyfd, &info, CORRECT_SIGNAL, 0, EBADF},
	{&pidfd, &info, DIFFERENT_SIGNAL, 0, EINVAL},
	{&init_pidfd, &info, CORRECT_SIGNAL, 0, EPERM},
};

int  verify_pidfd_send_signal(unsigned int n)
{
	struct tcase *tc = &tcases[n];
pidfd_send_signal(*tc->fd, tc->signal, tc->siginf, tc->flags);
	if (tc->exp_err != TST_ERR) {
		tst_res(TFAIL | TTERRNO,
			"pidfd_send_signal() did not fail with %s but",
			tst_strerrno(tc->exp_err));
		return;
	}
	tst_res(TPASS | TTERRNO,
		"pidfd_send_signal() failed as expected");
}

void setup(void)
{
	struct passwd *pw;
	pidfd_send_signal_supported();
	pidfd = open("/proc/self", O_DIRECTORY | O_CLOEXEC);
	init_pidfd = open("/proc/1", O_DIRECTORY | O_CLOEXEC);
	dummyfd = open("dummy_file", O_RDWR | O_CREAT, 0664);
	if (getuid() == 0) {
		pw = getpwnam("nobody");
		setuid(pw->pw_uid);
	}
	memset(&info, 0, sizeof(info));
	info.si_signo = CORRECT_SIGNAL;
	info.si_code = SI_QUEUE;
	info.si_pid = getpid();
	info.si_uid = getuid();
}

void cleanup(void)
{
	if (dummyfd > 0)
		close(dummyfd);
	if (init_pidfd > 0)
		close(init_pidfd);
	if (pidfd > 0)
		close(pidfd);
}

void main(){
	setup();
	cleanup();
}
