#include "incl.h"
#define TEST_DIR	"test_dir"
#define TEST_DIR4	"test_dir4"
#define TEST_FILE	"test_file"
#define DIR_MODE	0755

unsigned int del_dir_fd, file_fd;

unsigned int invalid_fd = 999;

unsigned int dir_fd;

struct old_linux_dirent dirp;

struct tcase {
	unsigned int *fd;
	struct old_linux_dirent *dirp;
	unsigned int count;
	int exp_errno;
	char *desc;
} tcases[] = {
	{&del_dir_fd, &dirp, sizeof(struct old_linux_dirent), ENOENT, "directory deleted"},
	{&file_fd, &dirp, sizeof(struct old_linux_dirent), ENOTDIR, "not a directory"},
	{&invalid_fd, &dirp, sizeof(struct old_linux_dirent), EBADF, "invalid fd"},
	{&dir_fd, (struct old_linux_dirent *)-1,
	 sizeof(struct old_linux_dirent), EFAULT, "invalid buffer pointer"},
};

void setup(void)
{
	unsigned int i;
	mkdir(TEST_DIR, DIR_MODE);
	del_dir_fd = open(TEST_DIR, O_RDONLY | O_DIRECTORY);
	rmdir(TEST_DIR);
	file_fd = open(TEST_FILE, O_RDWR | O_CREAT, 0777);
	mkdir(TEST_DIR4, DIR_MODE);
	dir_fd = open(TEST_DIR4, O_RDONLY | O_DIRECTORY);
	for (i = 0; i < ARRAY_SIZE(tcases); i++) {
		if (tcases[i].exp_errno == EFAULT)
			tcases[i].dirp = tst_get_bad_addr(NULL);
	}
}

int  verify_readdir(unsigned int nr)
{
	struct tcase *tc = &tcases[nr];
	TST_EXP_FAIL(tst_syscall(__NR_readdir, *tc->fd, tc->dirp, tc->count),
			tc->exp_errno, "readdir() with %s", tc->desc);
}

void main(){
	setup();
	cleanup();
}
