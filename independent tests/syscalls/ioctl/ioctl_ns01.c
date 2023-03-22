#include "incl.h"
#define _GNU_SOURCE
#define STACK_SIZE (1024 * 1024)

char *child_stack;

void setup(void)
{
	int exists = access("/proc/self/ns/pid", F_OK);
	if (exists < 0)
		tst_res(TCONF, "namespace not available");
	child_stack = ltp_alloc_stack(STACK_SIZE);
	if (!child_stack)
		tst_brk(TBROK|TERRNO, "stack alloc");
}

void cleanup(void)
{
	free(child_stack);
}

void test_ns_get_parent(void)
{
	int fd, parent_fd;
	fd = open("/proc/self/ns/pid", O_RDONLY);
	parent_fd = ioctl(fd, NS_GET_PARENT);
	if (parent_fd == -1) {
		if (errno == ENOTTY)
			tst_brk(TCONF, "ioctl(NS_GET_PARENT) not implemented");
		if (errno == EPERM)
			tst_res(TPASS, "NS_GET_PARENT fails with EPERM");
		else
			tst_res(TFAIL | TERRNO, "unexpected ioctl error");
	} else {
		tst_res(TFAIL, "call to ioctl succeded");
	}
	close(fd);
}

int child(void *arg LTP_ATTRIBUTE_UNUSED)
{
	test_ns_get_parent();
	return 0;
}

void run(void)
{
	test_ns_get_parent();
	if (ltp_clone(CLONE_NEWPID | SIGCHLD, &child, 0,
		STACK_SIZE, child_stack) == -1)
		tst_brk(TBROK | TERRNO, "ltp_clone failed");
}

void main(){
	setup();
	cleanup();
}
