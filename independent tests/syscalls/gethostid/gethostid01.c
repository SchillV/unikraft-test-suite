#include "incl.h"
#ifdef HAVE_SETHOSTID

long origin;

long tc[] = {0x00000000, 0x0000ffff};

void run(unsigned int i)
{
	TST_EXP_PASS(sethostid(tc[i]), "set hostid to %ld", tc[i]);
gethostid();
	if (TST_RET == -1)
		tst_res(TFAIL | TTERRNO, "gethostid failed");
	if (tc[i] == TST_RET)
		tst_res(TPASS, "hostid is %ld, expected %ld", TST_RET, tc[i]);
	else
		tst_res(TFAIL, "hostid is %ld, expected %ld", TST_RET, tc[i]);
}

void setup(void)
{
gethostid();
	if (TST_RET == -1)
		tst_brk(TFAIL | TTERRNO, "gethostid failed");
	tst_res(TINFO, "get original hostid: %ld", origin = TST_RET);
}

void cleanup(void)
{
	TST_EXP_PASS(sethostid(origin), "set hostid to %ld", origin);
}

void main(){
	setup();
	cleanup();
}
