#include "incl.h"
#define _GNU_SOURCE

void *child_stack;

int sysctl_net = -1;

int sysctl_net_new = -1;

const char sysctl_path[] = "/proc/sys/net/ipv4/conf/lo/tag";

const char sysctl_path_def[] = "/proc/sys/net/ipv4/conf/default/tag";

int flags = CLONE_NEWNET | CLONE_VM | SIGCHLD;

void setup(void)
{
	child_stack = malloc(CHILD_STACK_SIZE);
}

void cleanup(void)
{
	if (sysctl_net != -1)
		fprintf(sysctl_path, "%d", sysctl_net);
	free(child_stack);
}

int newnet(void *arg LTP_ATTRIBUTE_UNUSED)
{
	file_scanf(sysctl_path, "%d", &sysctl_net_new);
	tst_syscall(__NR_exit, 0);
	return 0;
}

long clone_child(void)
{
ltp_clone(flags, newnet, NULL, CHILD_STACK_SIZE, child_stack);
	if (TST_RET == -1 && TST_ERR == EINVAL)
		tst_brk(TCONF, "CONFIG_NET_NS was disabled");
	if (TST_RET == -1)
		tst_brk(TBROK | TTERRNO, "clone(CLONE_NEWNET) failed");
	return TST_RET;
}

void do_test(void)
{
	int def_val;
	tst_res(TINFO, "create clone in a new netns with 'CLONE_NEWNET' flag");
	file_scanf(sysctl_path, "%d", &sysctl_net);
	fprintf(sysctl_path, "%d", sysctl_net + 1);
	clone_child();
	tst_reap_children();
	if (sysctl_net_new == (sysctl_net + 1)) {
		tst_res(TFAIL, "sysctl params equal: %s=%d",
			sysctl_path, sysctl_net_new);
	}
	file_scanf(sysctl_path_def, "%d", &def_val);
	if (sysctl_net_new != def_val) {
		tst_res(TFAIL, "netns param init to non-default value %d",
			sysctl_net_new);
	}
	fprintf(sysctl_path, "%d", sysctl_net);
	tst_res(TPASS, "sysctl params differ in new netns");
}

void main(){
	setup();
	cleanup();
}
