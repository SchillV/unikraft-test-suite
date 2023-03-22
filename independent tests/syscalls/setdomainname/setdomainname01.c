#include "incl.h"

void do_test(void)
{
	char *new = TST_VALID_DOMAIN_NAME;
	

char tmp[_UTSNAME_DOMAIN_LENGTH];
do_setdomainname(new, strlen(new));
	if (TST_RET != 0)
		tst_brk(TFAIL | TTERRNO, "set" SYSCALL_NAME "() failed: %d", TST_ERR);
	if (GET_SYSCALL(tmp, sizeof(tmp)) != 0)
		tst_brk(TFAIL | TERRNO, "get" SYSCALL_NAME "() failed");
	if (strcmp(tmp, new))
		tst_res(TFAIL, "get" SYSCALL_NAME "() returned wrong domainname: '%s'", tmp);
	else
		tst_res(TPASS, "set" SYSCALL_NAME "() succeed");
}

