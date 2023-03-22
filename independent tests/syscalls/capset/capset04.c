#include "incl.h"

struct __user_cap_header_struct *header;

struct __user_cap_data_struct *data;

pid_t child_pid;

int  verify_capset(
{
	child_pid = fork();
	if (!child_pid)
		pause();
	tst_res(TINFO, "Test capset() for a different process");
	header->pid = child_pid;
	TST_EXP_FAIL(tst_syscall(__NR_capset, header, data), EPERM, "capset()");
	kill(child_pid, SIGTERM);
	wait(NULL);
}

void setup(void)
{
	header->version = 0x20080522;
tst_syscall(__NR_capget, header, data);
	if (TST_RET == -1)
		tst_brk(TBROK | TTERRNO, "capget data failed");
}

void main(){
	setup();
	cleanup();
}
