#include "incl.h"
#define _GNU_SOURCE
#define TESTDIR "testdir"
#define TEST_APP "execveat_errno"
#define TEST_SYMLINK "execveat_symlink"
#define TEST_REL_APP TESTDIR"/"TEST_APP
#define TEST_ERL_SYMLINK TESTDIR"/"TEST_SYMLINK

int bad_fd = -1, fd;

char app_abs_path[512], app_sym_path[512];

struct tcase {
	int *fd;
	char *pathname;
	int flag;
	int exp_err;
} tcases[] = {
	{&bad_fd, "", AT_EMPTY_PATH, EBADF},
	{&fd, app_abs_path, -1, EINVAL},
	{&fd, app_sym_path, AT_SYMLINK_NOFOLLOW, ELOOP},
	{&fd, TEST_REL_APP, 0, ENOTDIR},
};

int  verify_execveat(unsigned int i)
{
	struct tcase *tc = &tcases[i];
	char *argv[2] = {TEST_APP, NULL};
	pid_t pid;
	pid = fork();
	if (pid == 0) {
execveat(*tc->fd, tc->pathname, argv, environ, tc->flag);
		if (tc->exp_err != TST_ERR) {
			tst_res(TFAIL | TTERRNO,
				"execveat() fails unexpectedly, expected: %s",
				tst_strerrno(tc->exp_err));
		} else {
			tst_res(TPASS | TTERRNO,
				"execveat() fails as expected");
		}
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
	sprintf(app_sym_path, "%s/%s", cur_dir_path, TEST_ERL_SYMLINK);
	symlink(TEST_REL_APP, TEST_ERL_SYMLINK);
	fd = open(TEST_REL_APP, O_PATH);
}

void cleanup(void)
{
	if (fd > 0)
		close(fd);
}

void main(){
	setup();
	cleanup();
}
