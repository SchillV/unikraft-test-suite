#include "incl.h"
struct test_case_t {
	int call;
	unsigned long args[3];
	char *desc;
} TC[] = {
	{SYS_SOCKET, {PF_INET, SOCK_STREAM, 0}, "TCP stream"},
	{SYS_SOCKET, {PF_UNIX, SOCK_DGRAM, 0}, "unix domain dgram"},
	{SYS_SOCKET, {AF_INET, SOCK_RAW, 6}, "Raw socket"},
	{SYS_SOCKET, {PF_INET, SOCK_DGRAM, 17}, "UDP dgram"}
};
int  verify_socketcall(unsigned int i)
{
tst_syscall(__NR_socketcall, TC[i].call, TC[i].args);
	if (TST_RET < 0) {
		tst_res(TFAIL | TTERRNO, "socketcall() for %s failed with %li",
			TC[i].desc, TST_RET);
		return;
	}
	tst_res(TPASS, "socketcall() for %s", TC[i].desc);
	close(TST_RET);
}

