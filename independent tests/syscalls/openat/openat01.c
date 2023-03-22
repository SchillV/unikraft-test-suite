#include "incl.h"
#define _GNU_SOURCE
#define TEST_FILE "test_file"
#define TEST_DIR "test_dir/"

int dir_fd, fd;
int fd_invalid = 100;
int fd_atcwd = AT_FDCWD;
char glob_path[256];

struct test_case {
	int *dir_fd;
	const char *pathname;
	int exp_ret;
	int exp_errno;
} test_cases[] = {
	{&dir_fd, TEST_FILE, 0, 0},
	{&dir_fd, glob_path, 0, 0},
	{&fd, TEST_FILE, -1, ENOTDIR},
	{&fd_invalid, TEST_FILE, -1, EBADF},
	{&fd_atcwd, TEST_DIR TEST_FILE, 0, 0}
};

int verify_openat(unsigned int n)
{
	struct test_case *tc = &test_cases[n];
	int ret;
	if (tc->exp_ret) {
		if (tc->exp_errno == ENOTDIR) {
			printf("[I] openat with a filefd instead of dirfd\n");
			ret = openat(*tc->dir_fd, tc->pathname, O_RDWR, 0600);
			return (errno == ENOTDIR);
		} else {
			printf("[I] openat with invalid fd\n");
			ret = openat(*tc->dir_fd, tc->pathname, O_RDWR, 0600);
			return (errno == EBADF);
		}
	}
	if (ret > 0)
		close(ret);
}

void setup(void)
{
	char buf[PATH_MAX];
	getcwd(buf, PATH_MAX);
	mkdir(TEST_DIR, 0700);
	dir_fd = open(TEST_DIR, __O_DIRECTORY);
	fd = open(TEST_DIR TEST_FILE, O_CREAT | O_RDWR, 0600);
	snprintf(glob_path, sizeof(glob_path), "%s/" TEST_DIR TEST_FILE, buf);
}

void cleanup(void)
{
	if (fd > 0)
		close(fd);
	if (dir_fd > 0)
		close(dir_fd);
}

void main(){
	setup();
	int ok=1;
	for(int i=0;i<5;i++)
		ok *= verify_openat(i);
	if(ok)
		printf("test succeeded\n");
	cleanup();
}
