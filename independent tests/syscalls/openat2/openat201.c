#include "incl.h"
#define TEST_FILE "test_file"
#define TEST_DIR "test_dir"

struct open_how *how;

struct open_how_pad *phow;

int dir_fd = -1, fd_atcwd = AT_FDCWD;

struct tcase {
	int *dfd;
	const char *pathname;
	uint64_t flags;
	uint64_t mode;
	uint64_t resolve;
	struct open_how **how;
	size_t size;
} tcases[] = {
	{&dir_fd, TEST_FILE, O_RDWR, S_IRWXU, 0, &how, sizeof(*how)},
	{&dir_fd, TEST_FILE, O_RDONLY, S_IRUSR, 0, &how, sizeof(*how)},
	{&dir_fd, TEST_FILE, O_WRONLY, S_IWUSR, 0, &how, sizeof(*how)},
	{&dir_fd, TEST_FILE, O_RDWR, S_IRWXU, RESOLVE_NO_XDEV, &how, sizeof(*how)},
	{&dir_fd, TEST_FILE, O_RDWR, S_IRWXU, RESOLVE_NO_MAGICLINKS, &how, sizeof(*how)},
	{&dir_fd, TEST_FILE, O_RDWR, S_IRWXU, RESOLVE_NO_SYMLINKS, &how, sizeof(*how)},
	{&dir_fd, TEST_FILE, O_RDWR, S_IRWXU, RESOLVE_BENEATH, &how, sizeof(*how)},
	{&dir_fd, TEST_FILE, O_RDWR, S_IRWXU, RESOLVE_IN_ROOT, &how, sizeof(*how)},
	{&fd_atcwd, TEST_FILE, O_RDWR, S_IRWXU, 0, &how, sizeof(*how)},
	{&fd_atcwd, TEST_FILE, O_RDONLY, S_IRUSR, 0, &how, sizeof(*how)},
	{&fd_atcwd, TEST_FILE, O_WRONLY, S_IWUSR, 0, &how, sizeof(*how)},
	{&fd_atcwd, TEST_FILE, O_RDWR, S_IRWXU, RESOLVE_NO_XDEV, &how, sizeof(*how)},
	{&fd_atcwd, TEST_FILE, O_RDWR, S_IRWXU, RESOLVE_NO_MAGICLINKS, &how, sizeof(*how)},
	{&fd_atcwd, TEST_FILE, O_RDWR, S_IRWXU, RESOLVE_NO_SYMLINKS, &how, sizeof(*how)},
	{&fd_atcwd, TEST_FILE, O_RDWR, S_IRWXU, RESOLVE_BENEATH, &how, sizeof(*how)},
	{&fd_atcwd, TEST_FILE, O_RDWR, S_IRWXU, RESOLVE_IN_ROOT, (struct open_how **)&phow, sizeof(*how) + 8},
};

void cleanup(void)
{
	if (dir_fd != -1)
		close(dir_fd);
}

void setup(void)
{
	openat2_supported_by_kernel();
	phow->pad = 0x00;
	mkdir(TEST_DIR, 0700);
	dir_fd = open(TEST_DIR, O_DIRECTORY);
}

void run(unsigned int n)
{
	int fd;
	struct stat file_stat;
	struct tcase *tc = &tcases[n];
	struct open_how *myhow = *tc->how;
	myhow->flags = tc->flags | O_CREAT;
	myhow->mode = tc->mode;
	myhow->resolve = tc->resolve;
fd = openat2(*tc->dfd, tc->pathname, myhow, tc->size);
	if (fd < 0) {
		tst_res(TFAIL | TTERRNO, "openat2() failed (%d)", n);
		return;
	}
	fstat(fd, &file_stat);
	if (file_stat.st_size == 0)
		tst_res(TPASS, "openat2() passed (%d)", n);
	else
		tst_res(TFAIL, "fstat() didn't work as expected (%d)", n);
	close(fd);
}

void main(){
	setup();
	cleanup();
}
