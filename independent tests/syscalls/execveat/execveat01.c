#include "incl.h"
#define _GNU_SOURCE
#define TESTDIR "testdir"
#define TEST_APP "execveat_child"
#define TEST_REL_APP TESTDIR"/"TEST_APP

int fd1, fd4;

int fd2 = AT_FDCWD, fd3 = -1;

char app_abs_path[512];

struct tcase {
	int *fd;
	char *pathname;
	int flag;
} tcases[] = {
	{&fd1, TEST_APP, 0},
	{&fd2, TEST_REL_APP, 0},
	{&fd3, app_abs_path, 0},
	{&fd4, "", AT_EMPTY_PATH},
};

int  verify_execveat(unsigned int i)
{
	struct tcase *tc = &tcases[i];
	char *argv[2] = {TEST_APP, NULL};
	pid_t pid;
	pid = fork();
	if (pid == 0) {
execveat(*tc->fd, tc->pathname, argv, environ, tc->flag);
		tst_res(TFAIL | TTERRNO, "execveat() returns unexpected errno");
	}
}

void setup(void)
{
	char cur_dir_path[512];
	check_execveat();
	mkdir(TESTDIR, 0777);
	cp(TEST_APP, TEST_REL_APP);
	getcwd(cur_dir_path, sizeofcur_dir_path, sizeofcur_dir_path));
	sprintf(app_abs_path, "%s/%s", cur_dir_path, TEST_REL_APP);
	fd1 = open(TESTDIR, O_DIRECTORY);
	fd4 = open(TEST_REL_APP, O_PATH);
}

void cleanup(void)
{
	if (fd1 > 0)
		close(fd1);
	if (fd4 > 0)
		close(fd4);
}

void main(){
	setup();
	cleanup();
}
