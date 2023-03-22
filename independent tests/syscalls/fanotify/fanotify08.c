#include "incl.h"
#define _GNU_SOURCE
#ifdef HAVE_SYS_FANOTIFY_H

int fd_notify;

void test_init_bit(unsigned int fan_bit,
			unsigned int fd_bit, char *msg)
{
	int ret;
	fd_notify = fanotify_init(FAN_CLASS_NOTIF|fan_bit, O_RDONLY);
	ret = fcntl(fd_notify, F_GETFD);
	if ((ret & FD_CLOEXEC) == fd_bit)
		tst_res(TPASS, "%s", msg);
	else
		tst_res(TFAIL, "%s", msg);
	close(fd_notify);
}

void run(unsigned int i)
{
	switch (i) {
	case 0:
		test_init_bit(0, 0, "not set close_on_exec");
		break;
	case 1:
		test_init_bit(FAN_CLOEXEC, FD_CLOEXEC, "set close_on_exec");
		break;
	}
}

void cleanup(void)
{
	if (fd_notify > 0)
		close(fd_notify);
}

void main(){
	setup();
	cleanup();
}
