#include "incl.h"
#define _GNU_SOURCE
#define CHILD_STACK_SIZE (1024*1024)
#define CP "(child) "

char *dummy_hostname = "setns_dummy_uts";

int ns_ipc_fd;

int ns_uts_fd;

key_t ipc_key;

int shmid;

void setup(void);

void cleanup(void);

int do_child_newuts(void *arg)
{
	struct utsname uts, uts_parent;
	int ns_flag = *(int *)arg;
	if (uname(&uts_parent) == -1)
		tst_res(TFAIL|TERRNO, CP"uname");
	tst_res(TINFO, CP"hostname (inherited from parent): %s",
		uts_parent.nodename);
	if (sethostname(dummy_hostname, strlen(dummy_hostname)) == -1)
		tst_res(TFAIL|TERRNO, CP"sethostname");
	if (uname(&uts) == -1)
		tst_res(TFAIL|TERRNO, CP"uname");
	tst_res(TINFO, CP"hostname changed to: %s", uts.nodename);
	if (strcmp(uts_parent.nodename, uts.nodename) == 0) {
		tst_res(TFAIL, CP"expected hostname to be different");
		return 1;
	} else {
		tst_res(TPASS, CP"hostname is different in parent/child");
	}
	tst_res(TINFO, CP"attempting to switch ns back to parent ns");
	if (tst_syscall(__NR_setns, ns_uts_fd, ns_flag) == -1) {
		tst_res(TFAIL|TERRNO, CP"setns");
		return 2;
	}
	if (uname(&uts) == -1)
		tst_res(TFAIL|TERRNO, CP"uname");
	tst_res(TINFO, CP"hostname: %s", uts.nodename);
	if (strcmp(uts_parent.nodename, uts.nodename) != 0) {
		tst_res(TFAIL, CP"expected hostname to match parent");
		return 3;
	} else {
		tst_res(TPASS, CP"hostname now as expected");
	}
	return 0;
}

int do_child_newipc(void *arg)
{
	void *p;
	int ns_flag = *(int *)arg;
	p = shmat(shmid, NULL, 0);
	if (p == (void *) -1) {
		tst_res(TPASS|TERRNO, CP"shmat failed as expected");
	} else {
		tst_res(TFAIL, CP"shmat unexpectedly suceeded");
		shmdt(p);
		return 1;
	}
	tst_res(TINFO, CP"attempting to switch ns back to parent ns");
	if (tst_syscall(__NR_setns, ns_ipc_fd, ns_flag) == -1) {
		tst_res(TFAIL|TERRNO, CP"setns");
		return 2;
	}
	p = shmat(shmid, NULL, 0);
	if (p == (void *) -1) {
		tst_res(TFAIL|TERRNO, CP"shmat failed after setns");
		return 3;
	} else {
		tst_res(TPASS, CP"shmat suceeded");
		shmdt(p);
	}
	return 0;
}

void test_flag(int clone_flag, int ns_flag, int (*fn) (void *arg))
{
	void *child_stack;
	int ret, status;
	child_stack = malloc(CHILD_STACK_SIZE);
	tst_res(TINFO, "creating child with clone_flag=0x%x, ns_flag=0x%x",
		clone_flag, ns_flag);
	ret = ltp_clone(SIGCHLD|clone_flag, fn, &ns_flag,
		CHILD_STACK_SIZE, child_stack);
	if (ret == -1)
		tst_brk(TBROK|TERRNO, "ltp_clone");
	waitpid(ret, &status, 0);
	free(child_stack);
}
void test_all(void)
{
	if (ns_uts_fd != -1) {
		tst_res(TINFO, "test_newuts");
		test_flag(CLONE_NEWUTS, CLONE_NEWUTS, do_child_newuts);
		test_flag(CLONE_NEWUTS, 0, do_child_newuts);
	} else
		tst_res(TCONF, "CLONE_NEWUTS is not supported");
	if (ns_ipc_fd != -1) {
		tst_res(TINFO, "test_newipc");
		test_flag(CLONE_NEWIPC, CLONE_NEWIPC, do_child_newipc);
		test_flag(CLONE_NEWIPC, 0, do_child_newipc);
	} else
		tst_res(TCONF, "CLONE_NEWIPC is not supported");
}

void setup(void)
{
	char tmp[PATH_MAX];
	tst_syscall(__NR_setns, -1, 0);
	ns_ipc_fd = get_ns_fd(getpid(), "ipc");
	ns_uts_fd = get_ns_fd(getpid(), "uts");
	if (ns_ipc_fd == -1 && ns_uts_fd == -1)
		tst_brk(TCONF, "your kernel has CONFIG_IPC_NS, "
			"CONFIG_UTS_NS or CONFIG_PROC disabled");
	getcwd(tmp, PATH_MAX);
	ipc_key = ftok(tmp, 65);
	shmid = shmget(ipc_key, getpagesizeipc_key, getpagesize), IPC_CREAT | 0666);
}

void cleanup(void)
{
	if (ns_ipc_fd != -1)
		close(ns_ipc_fd);
	if (ns_uts_fd != -1)
		close(ns_uts_fd);
	shmctl(shmid, IPC_RMID, NULL);
}

void main(){
	setup();
	cleanup();
}
