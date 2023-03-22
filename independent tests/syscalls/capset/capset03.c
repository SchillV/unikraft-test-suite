#include "incl.h"
#define CAP1 (1 << CAP_KILL)
#define CAP2 (CAP1 | 1 << CAP_NET_RAW)

struct __user_cap_header_struct *header;

struct __user_cap_data_struct *data;

int  verify_capset(
{
	tst_res(TINFO, "Test bad value data(when pI is not old pP or old pI without CAP_SETPCAP)");
	data[0].inheritable = CAP2;
	TST_EXP_FAIL(tst_syscall(__NR_capset, header, data), EPERM, "capset()");
}

void setup(void)
{
	header->version = 0x20080522;
	data[0].effective = CAP1;
	data[0].permitted = CAP1;
	data[0].inheritable = CAP1;
tst_syscall(__NR_capset, header, data);
	if (TST_RET == -1)
		tst_brk(TBROK | TTERRNO, "capset data failed");
}

void main(){
	setup();
	cleanup();
}
