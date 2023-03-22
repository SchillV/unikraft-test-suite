#include "incl.h"
#define _GNU_SOURCE
#define STACK_SIZE	(1024*1024)

int pid1;

int pid2;

void *stack;

struct tcase {
	int clone_type;
	int kcmp_type;
} tcases[] = {
	{CLONE_VM, KCMP_VM},
	{CLONE_FS, KCMP_FS},
	{CLONE_IO, KCMP_IO},
	{CLONE_SYSVSEM, KCMP_SYSVSEM}
};

void setup(void)
{
	stack = malloc(STACK_SIZE);
}

void cleanup(void)
{
	free(stack);
}

int do_child(void *arg)
{
	pid2 = getpid();
kcmp(pid1, pid2, *(int *)arg, 0, 0);
	if (TST_RET == -1) {
		tst_res(TFAIL | TTERRNO, "kcmp() failed unexpectedly");
		return 0;
	}
	if (TST_RET == 0)
		tst_res(TPASS, "kcmp() returned the expected value");
	else
		tst_res(TFAIL, "kcmp() returned the unexpected value");
	return 0;
}

int  verify_kcmp(unsigned int n)
{
	int res;
	struct tcase *tc = &tcases[n];
	pid1 = getpid();
	res = ltp_clone(tc->clone_type | SIGCHLD, do_child, &tc->kcmp_type,
			STACK_SIZE, stack);
	if (res == -1)
		tst_res(TFAIL | TERRNO, "clone() Failed");
}

void main(){
	setup();
	cleanup();
}
