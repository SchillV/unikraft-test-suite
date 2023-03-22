#include "incl.h"

struct sockaddr_in si;

int  verify_socketcall(
{
	unsigned long args[3];
	int s = -1;
	s = socket(AF_INET, SOCK_STREAM, 6);
	args[0] = s;
	args[1] = (unsigned long)&si;
	args[2] = sizeof(si);
tst_syscall(__NR_socketcall, SYS_BIND, args);
	if (TST_RET < 0)
		tst_res(TFAIL | TTERRNO, "socketcall() for bind call failed with %ld", TST_RET);
	else
		tst_res(TPASS, "socketcall() for bind call passed, returned %ld", TST_RET);
	args[1] = 1;
	args[2] = 0;
tst_syscall(__NR_socketcall, SYS_LISTEN, args);
	if (TST_RET < 0)
		tst_res(TFAIL | TTERRNO, "socketcall() for listen call failed with %ld", TST_RET);
	else
		tst_res(TPASS, "socketcall() for listen call passed, returned %ld", TST_RET);
	close(s);
}

void setup(void)
{
	si.sin_family = AF_INET;
	si.sin_addr.s_addr = htons(INADDR_ANY);
	si.sin_port = 0;
}

void main(){
	setup();
	cleanup();
}
