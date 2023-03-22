#include "incl.h"
#ifndef _GNU_SOURCE
#define _GNU_SOURCE
#endif
#define TEST_APP "execve_child"

int nchild = 8;

char *opt_nchild;

void do_child(void)
{
	char *argv[3] = {TEST_APP, "canary", NULL};
	int ret = execve(TEST_APP, argv, __environ);
	printf("execve() returned unexpected errno: %d\n", errno);
}

void verify_execve()
{
	int i, ok=1;
	for (i = 0; i < nchild; i++) {
		if (fork() == 0){
			do_child();
			ok=0;
		}
	}
	if(ok){
		printf("test succeeded\n");
		exit(0);
	}
}

void setup(void)
{
	if (opt_nchild)
		nchild = strtol(opt_nchild, 1, INT_MAX);
}

void main(){
	setup();
	verify_execve();
}
