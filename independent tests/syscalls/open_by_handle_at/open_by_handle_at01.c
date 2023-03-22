#include "incl.h"
#define _GNU_SOURCE
#define TEST_FILE "test_file"
#define TEST_DIR "test_dir"

int dir_fd, fd_atcwd = AT_FDCWD, file_fd;

struct file_handle *f_fhp, *d_fhp, *at_fhp;

struct file_handle *f_fhp, *d_fhp, *at_fhp;

struct tcase {
	int *dfd;
	struct file_handle **fhp;
	int flags;
} tcases[] = {
	{&dir_fd, &d_fhp, O_RDWR},
	{&dir_fd, &d_fhp, O_RDONLY},
	{&dir_fd, &d_fhp, O_WRONLY},
	{&file_fd, &f_fhp, O_RDWR},
	{&file_fd, &f_fhp, O_RDONLY},
	{&file_fd, &f_fhp, O_WRONLY},
	{&fd_atcwd, &at_fhp, O_RDWR},
	{&fd_atcwd, &at_fhp, O_RDONLY},
	{&fd_atcwd, &at_fhp, O_WRONLY},
};

void cleanup(void)
{
	close(dir_fd);
	close(file_fd);
}

void setup(void)
{
	int mount_id;
	mkdir(TEST_DIR, 0700);
	dir_fd = open(TEST_DIR, O_DIRECTORY);
	chdir(TEST_DIR);
	touch(TEST_FILE, 0600, NULL);
	file_fd = open("foo_file", O_RDWR | O_CREAT);
	f_fhp = allocate_file_handle(AT_FDCWD, TEST_FILE);
	d_fhp = allocate_file_handle(AT_FDCWD, TEST_FILE);
	at_fhp = allocate_file_handle(AT_FDCWD, TEST_FILE);
name_to_handle_at(file_fd, "", f_fhp, &mount_id, AT_EMPTY_PATH);
	if (TST_RET) {
		tst_res(TFAIL | TTERRNO, "name_to_handle_at() failed");
		return;
	}
name_to_handle_at(dir_fd, TEST_FILE, d_fhp, &mount_i;
			       AT_EMPTY_PATH | AT_SYMLINK_FOLLOW));
	if (TST_RET) {
		tst_res(TFAIL | TTERRNO, "name_to_handle_at() failed");
		return;
	}
name_to_handle_at(AT_FDCWD, TEST_FILE, at_fhp, &mount_i;
			       AT_EMPTY_PATH | AT_SYMLINK_FOLLOW));
	if (TST_RET) {
		tst_res(TFAIL | TTERRNO, "name_to_handle_at() failed");
		return;
	}
}

void run(unsigned int n)
{
	struct tcase *tc = &tcases[n];
	struct stat file_stat;
	int fd;
fd = open_by_handle_at(*tc->dfd, *tc->fhp, tc->flags);
	if (fd < 0) {
		tst_res(TFAIL | TTERRNO, "open_by_handle_at() failed (%d)", n);
		return;
	}
	fstat(fd, &file_stat);
	if (file_stat.st_size == 0 || (tc->fhp == &f_fhp))
		tst_res(TPASS, "open_by_handle_at() passed (%d)", n);
	else
		tst_res(TFAIL, "fstat() didn't work as expected (%d)", n);
	close(fd);
}

void main(){
	setup();
	cleanup();
}
