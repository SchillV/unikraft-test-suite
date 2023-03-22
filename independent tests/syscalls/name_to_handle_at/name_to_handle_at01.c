#include "incl.h"
#define _GNU_SOURCE
#define TEST_FILE "test_file"
#define TEST_DIR "test_dir"

int dir_fd, fd_atcwd = AT_FDCWD, file_fd;

struct file_handle *fhp;

struct tcase {
	int *dfd;
	const char *pathname;
	int name_flags;
	int flags;
} tcases[] = {
	{&dir_fd, TEST_FILE, 0, O_RDWR},
	{&dir_fd, TEST_FILE, 0, O_RDONLY},
	{&dir_fd, TEST_FILE, 0, O_WRONLY},
	{&dir_fd, TEST_FILE, AT_EMPTY_PATH, O_RDWR},
	{&dir_fd, TEST_FILE, AT_EMPTY_PATH, O_RDONLY},
	{&dir_fd, TEST_FILE, AT_EMPTY_PATH, O_WRONLY},
	{&dir_fd, TEST_FILE, AT_SYMLINK_FOLLOW, O_RDWR},
	{&dir_fd, TEST_FILE, AT_SYMLINK_FOLLOW, O_RDONLY},
	{&dir_fd, TEST_FILE, AT_SYMLINK_FOLLOW, O_WRONLY},
	{&dir_fd, TEST_FILE, AT_EMPTY_PATH | AT_SYMLINK_FOLLOW, O_RDWR},
	{&dir_fd, TEST_FILE, AT_EMPTY_PATH | AT_SYMLINK_FOLLOW, O_RDONLY},
	{&dir_fd, TEST_FILE, AT_EMPTY_PATH | AT_SYMLINK_FOLLOW, O_WRONLY},
	{&dir_fd, "", AT_EMPTY_PATH, O_RDONLY},
	{&file_fd, "", AT_EMPTY_PATH, O_RDONLY},
	{&fd_atcwd, TEST_FILE, 0, O_RDWR},
	{&fd_atcwd, TEST_FILE, 0, O_RDONLY},
	{&fd_atcwd, TEST_FILE, 0, O_WRONLY},
	{&fd_atcwd, TEST_FILE, AT_EMPTY_PATH, O_RDWR},
	{&fd_atcwd, TEST_FILE, AT_EMPTY_PATH, O_RDONLY},
	{&fd_atcwd, TEST_FILE, AT_EMPTY_PATH, O_WRONLY},
	{&fd_atcwd, TEST_FILE, AT_SYMLINK_FOLLOW, O_RDWR},
	{&fd_atcwd, TEST_FILE, AT_SYMLINK_FOLLOW, O_RDONLY},
	{&fd_atcwd, TEST_FILE, AT_SYMLINK_FOLLOW, O_WRONLY},
	{&fd_atcwd, TEST_FILE, AT_EMPTY_PATH | AT_SYMLINK_FOLLOW, O_RDWR},
	{&fd_atcwd, TEST_FILE, AT_EMPTY_PATH | AT_SYMLINK_FOLLOW, O_RDONLY},
	{&fd_atcwd, TEST_FILE, AT_EMPTY_PATH | AT_SYMLINK_FOLLOW, O_WRONLY},
	{&fd_atcwd, "", AT_EMPTY_PATH, O_RDONLY},
};

void cleanup(void)
{
	close(dir_fd);
	close(file_fd);
}

void setup(void)
{
	mkdir(TEST_DIR, 0700);
	dir_fd = open(TEST_DIR, O_DIRECTORY);
	chdir(TEST_DIR);
	touch(TEST_FILE, 0600, NULL);
	file_fd = open("foo_file", O_RDWR | O_CREAT);
	fhp = allocate_file_handle(AT_FDCWD, TEST_FILE);
}

void run(unsigned int n)
{
	struct tcase *tc = &tcases[n];
	struct stat file_stat;
	int fd, mount_id;
name_to_handle_at(*tc->dfd, tc->pathname, fhp, &mount_i;
			       tc->name_flags));
	if (TST_RET) {
		tst_res(TFAIL | TTERRNO, "name_to_handle_at() failed (%d)", n);
		return;
	}
fd = open_by_handle_at(*tc->dfd, fhp, tc->flags);
	if (fd < 0) {
		tst_res(TFAIL | TTERRNO, "open_by_handle_at() failed (%d)", n);
		return;
	}
	fstat(fd, &file_stat);
	if (file_stat.st_size == 0 || !tc->pathname[0])
		tst_res(TPASS, "name_to_handle_at() passed (%d)", n);
	else
		tst_res(TFAIL, "fstat() didn't work as expected (%d)", n);
	close(fd);
}

void main(){
	setup();
	cleanup();
}
