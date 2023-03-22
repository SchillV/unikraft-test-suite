#include "incl.h"
#define _GNU_SOURCE

int requests[] = {NS_GET_PARENT, NS_GET_USERNS,
	NS_GET_OWNER_UID, NS_GET_NSTYPE};

void test_request(unsigned int n)
{
	int request = requests[n];
	int fd, ns_fd;
	fd = open(".", O_RDONLY);
	ns_fd = ioctl(fd, request);
	if (ns_fd == -1) {
		if (errno == ENOTTY)
			tst_res(TPASS, "request failed with ENOTTY");
		else
			tst_res(TFAIL | TERRNO, "unexpected ioctl error");
	} else {
		tst_res(TFAIL, "request success for invalid fd");
		close(ns_fd);
	}
	close(fd);
}

void main(){
	setup();
	cleanup();
}
