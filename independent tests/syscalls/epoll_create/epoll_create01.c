#include "incl.h"

int tc[] = {1, INT_MAX};

void run(unsigned int n)
{
	TST_EXP_FD(do_epoll_create(tc[n]), "epoll_create(%d)", tc[n]);
	if (!TST_PASS)
		return;
	close(TST_RET);
}

void main(){
	setup();
	cleanup();
}
