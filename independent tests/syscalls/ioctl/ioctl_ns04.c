#include "incl.h"
#define _GNU_SOURCE

void setup(void)
{
	int exists = access("/proc/self/ns/user", F_OK);
	if (exists < 0)
		tst_res(TCONF, "namespace not available");
}

void run(void)
{
	int fd, parent_fd;
	fd = open("/proc/self/ns/user", O_RDONLY);
	parent_fd = ioctl(fd, NS_GET_USERNS);
	if (parent_fd == -1) {
		if (errno == ENOTTY)
			tst_brk(TCONF, "ioctl(NS_GET_USERNS) not implemented");
		if (errno == EPERM)
			tst_res(TPASS, "NS_GET_USERNS fails with EPERM");
		else
			tst_res(TFAIL | TERRNO, "unexpected ioctl error");
	} else {
		tst_res(TFAIL, "call to ioctl succeded");
	}
	close(fd);
}

void main(){
	setup();
	cleanup();
}
