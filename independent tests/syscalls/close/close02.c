#include "incl.h"

int run(void)
{
	int ret = close(-1);
	return -ret;
}

void main(){
	int res = run();
	if(res == 1)
		printf("test succeeded\n");
	else
		printf("test failed\n");
}
