#include "incl.h"
#define _GNU_SOURCE
#define MNT_POINT	"mntpoint"
#define TEST_DIR	"mntpoint/test_dir"
#define DIR_MODE	(S_IRUSR|S_IWUSR|S_IXUSR|S_IRGRP| \
			 S_IXGRP|S_IROTH|S_IXOTH)

int dir_fd;

int cur_fd = AT_FDCWD;

char test_dir[PATH_MAX] = ".";

struct tcase {
	int *dirfd;
	char *pathname;
	int exp_errno;
} tcases[] = {
	{&dir_fd, TEST_DIR, EROFS},
	{&cur_fd, TEST_DIR, EROFS},
	{&dir_fd, test_dir, ELOOP},
	{&cur_fd, test_dir, ELOOP},
};

void setup(void)
{
	unsigned int i;
	dir_fd = open(".", O_DIRECTORY);
	mkdir("test_eloop", DIR_MODE);
	symlink("../test_eloop", "test_eloop/test_eloop");
	 * NOTE: the ELOOP test is written based on that the consecutive
	 * symlinks limits in kernel is hardwired to 40.
	 */
	for (i = 0; i < 43; i++)
		strcat(test_dir, "/test_eloop");
}

int  mkdirat_verify(unsigned int i)
{
	struct tcase *test = &tcases[i];
mkdirat(*test->dirfd, test->pathname, 0777);
	if (TST_RET != -1) {
		tst_res(TFAIL, "mkdirat() succeeded unexpectedly (%li)",
			TST_RET);
		return;
	}
	if (TST_ERR == test->exp_errno) {
		tst_res(TPASS | TTERRNO, "mkdirat() failed as expected");
		return;
	}
	tst_res(TFAIL | TTERRNO,
		"mkdirat() failed unexpectedly; expected: %d - %s",
		test->exp_errno, tst_strerrno(test->exp_errno));
}

void main(){
	setup();
	cleanup();
}
