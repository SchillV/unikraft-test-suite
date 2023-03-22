#include "incl.h"
#define _GNU_SOURCE

int fd = -1;

struct tcase {
	int coe_flag;
	char *desc;
} tcases[] = {
	{0, "dup3(1, 4, 0)"},
	{O_CLOEXEC, "dup3(1, 4, O_CLOEXEC)"},
};

void cleanup(void)
{
	if (fd > -1)
		close(fd);
}

void run(unsigned int i)
{
	int ret;
	struct tcase *tc = tcases + i;
	TST_EXP_FD_SILENT(dup3(1, 4, tc->coe_flag), "dup3(1, 4, %d)", tc->coe_flag);
	fd = TST_RET;
	ret = fcntl(fd, F_GETFD);
	if (tc->coe_flag) {
		if (ret & FD_CLOEXEC)
			tst_res(TPASS, "%s set close-on-exec flag", tc->desc);
		else
			tst_res(TFAIL, "%s set close-on-exec flag", tc->desc);
	} else {
		if (ret & FD_CLOEXEC)
			tst_res(TFAIL, "%s set close-on-exec flag", tc->desc);
		else
			tst_res(TPASS, "%s set close-on-exec flag", tc->desc);
	}
};

void main(){
	setup();
	cleanup();
}
