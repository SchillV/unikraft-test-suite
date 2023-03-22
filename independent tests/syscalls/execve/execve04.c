#include "incl.h"
#ifndef _GNU_SOURCE
#define _GNU_SOURCE
#endif
#define TEST_APP "execve_child"

void do_child(void);

int  verify_execve()
{
	pid_t pid;
	char *argv[2] = {TEST_APP, NULL};
	pid = fork();
	if (pid == 0)
		do_child();
	int ret = execve(TEST_APP, argv, __environ);
	if (errno != ETXTBSY){
		printf("execve succeeded, expected failure\n");
		return 0;
	}
	else{
		printf("execve failed as expected");
		return 1;
	}
}

void do_child(void)
{
	int fd = open(TEST_APP, O_WRONLY);
	close(fd);
	exit(0);
}

void main(){
	if(verify_execve())
		printf("test succeeded\n");
}
