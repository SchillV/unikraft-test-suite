#include "incl.h"
#define _GNU_SOURCE
#define TEST_APP "execveat_child"
#define TEST_FILE_PATH OVL_MNT"/"TEST_APP

const char mntpoint[] = OVL_BASE_MNTPOINT;

void do_child(void)
{
	char *argv[2] = {TEST_FILE_PATH, NULL};
	int fd;
	cp(TEST_APP, TEST_FILE_PATH);
	fd = open(TEST_FILE_PATH, O_PATH);
	unlink(TEST_FILE_PATH);
execveat(fd, "", argv, environ, AT_EMPTY_PATH);
	tst_res(TFAIL | TTERRNO, "execveat() returned unexpected errno");
}

int  verify_execveat(
{
	pid_t pid;
	pid = fork();
	if (pid == 0)
		do_child();
}

void setup(void)
{
	check_execveat();
}

void main(){
	setup();
	cleanup();
}
