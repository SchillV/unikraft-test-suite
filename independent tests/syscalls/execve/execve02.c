#include "incl.h"
#ifndef _GNU_SOURCE
#define _GNU_SOURCE
#endif
#define TEST_APP "execve_child"
#define USER_NAME "nobody"

uid_t nobody_uid;

void do_child(void)
{
	char *argv[2] = {"canary", NULL};
	seteuid(nobody_uid);
	int ret = execve(TEST_APP, argv, __environ);
	if (!ret)
		printf("execve() passed unexpectedly\n");
	if (errno != EACCES){
		printf("execve() failed unexpectedly, error number %d\n", errno);
		return;
	}
	printf("execve() failed expectedly\ntest succeeded\n");
	exit(0);
}

int  verify_execve()
{
	pid_t pid = fork();
	if (pid == 0)
		do_child();
}

void setup(void)
{
	struct passwd *pwd;
	chmod(TEST_APP, 0700);
	pwd = getpwnam(USER_NAME);
	nobody_uid = pwd->pw_uid;
}

void main(){
	setup();
	verify_execve();
}
