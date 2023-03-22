#include "incl.h"
#define _GNU_SOURCE
#define STACK_SIZE (1024 * 1024)

char *child_stack;

void setup(void)
{
	int exists = access("/proc/self/ns/user", F_OK);
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

int child(void *arg LTP_ATTRIBUTE_UNUSED)
{
	TST_CHECKPOINT_WAIT(0);
	return 0;
}

void run(void)
{
	char child_namespace[30];
	pid_t pid = ltp_clone(CLONE_NEWUSER | SIGCHLD, &child, 0,
		STACK_SIZE, child_stack);
	if (pid == -1)
		tst_brk(TBROK | TERRNO, "ltp_clone failed");
	snprintf(child_namespace, sizeof(child_namespace), "/proc/%i/ns/user", pid);
	int my_fd, child_fd, parent_fd;
	my_fd = open("/proc/self/ns/user", O_RDONLY);
	child_fd = open(child_namespace, O_RDONLY);
	parent_fd = ioctl(child_fd, NS_GET_USERNS);
	if (parent_fd == -1) {
		TST_CHECKPOINT_WAKE(0);
		if (errno == ENOTTY)
			tst_brk(TCONF, "ioctl(NS_GET_USERNS) not implemented");
		tst_brk(TBROK | TERRNO, "ioctl(NS_GET_USERNS) failed");
	}
	struct stat my_stat, child_stat, parent_stat;
	fstat(my_fd, &my_stat);
	fstat(child_fd, &child_stat);
	fstat(parent_fd, &parent_stat);
	if (my_stat.st_ino != parent_stat.st_ino)
		tst_res(TFAIL, "parents have different inodes");
	else if (parent_stat.st_ino == child_stat.st_ino)
		tst_res(TFAIL, "child and parent have same inode");
	else
		tst_res(TPASS, "child and parent are consistent");
	close(my_fd);
	close(parent_fd);
	close(child_fd);
	TST_CHECKPOINT_WAKE(0);
}

void main(){
	setup();
	cleanup();
}
