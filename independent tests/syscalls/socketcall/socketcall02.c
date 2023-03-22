#include "incl.h"

unsigned long args_valid[3] = {PF_INET, SOCK_STREAM, 0};
struct test_case_t {
	int call;
	unsigned long *args;
	int exp_err;
	char *desc;
} TC[] = {
	{0, args_valid, EINVAL, "invalid call(<1)"},
	{21, args_valid, EINVAL, "invalid call(>20)"},
	{SYS_SOCKET, NULL, EFAULT, "invalid args address"},
};

int  verify_socketcall(unsigned int i)
{
	tst_res(TINFO, "%s", TC[i].desc);
tst_syscall(__NR_socketcall, TC[i].call, TC[i].args);
	if (TST_RET != -1) {
		tst_res(TFAIL, "socketcall() succeeded unexpectedly");
		return;
	}
	if (TST_ERR == TC[i].exp_err)
		tst_res(TPASS | TTERRNO, "socketcall() failed as expected ");
	else
		tst_res(TFAIL | TTERRNO, "socketcall fail expected %s got", tst_strerrno(TC[i].exp_err));
}

void setup(void)
{
	unsigned int i;
	for (i = 0; i < ARRAY_SIZE(TC); i++) {
		if (!TC[i].args)
			TC[i].args = tst_get_bad_addr(NULL);
	}
}

void main(){
	setup();
	cleanup();
}
