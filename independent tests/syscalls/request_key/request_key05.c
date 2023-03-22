#include "incl.h"
#define ATTEMPTS 0x100

void run(void)
{
	int i;
	tst_res(TINFO, "Requesting dead key");
	for (i = 0; i < ATTEMPTS; i++)
		tst_syscall(__NR_request_key, "dead", "abc", "abc", 0, 0, 0);
	tst_res(TPASS, "No crash after %d attempts", ATTEMPTS);
}

void main(){
	setup();
	cleanup();
}
