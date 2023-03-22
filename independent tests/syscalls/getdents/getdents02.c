#include "incl.h"
#define _GNU_SOURCE
#define DIR_MODE	(S_IRUSR|S_IWUSR|S_IXUSR|S_IRGRP| \
			 S_IXGRP|S_IROTH|S_IXOTH)
#define TEST_DIR	"test_dir"
char *TCID = "getdents02";

char *dirp;

size_t size;

char dirp1_arr[1];

char *dirp1 = dirp1_arr;

size_t size1 = 1;

int fd_inv = -5;

int fd;

int fd_file;

int fd_unlinked;

struct tcase {
	int *fd;
	char **dirp;
	size_t *size;
	int exp_errno;
} tcases[] = {
	{ &fd_inv, &dirp, &size, EBADF },
	{ &fd, &dirp1, &size1, EINVAL },
	{ &fd_file, &dirp, &size, ENOTDIR },
	{ &fd_unlinked, &dirp, &size, ENOENT },
};

void setup(void)
{
	getdents_info();
	size = tst_dirp_size();
	dirp = tst_alloc(size);
	fd = open(".", O_RDONLY);
	fd_file = open("test", O_CREAT | O_RDWR, 0644);
	mkdir(TEST_DIR, DIR_MODE);
	fd_unlinked = open(TEST_DIR, O_DIRECTORY);
	rmdir(TEST_DIR);
}

void run(unsigned int i)
{
	struct tcase *tc = tcases + i;
tst_getdents(*tc->fd, *tc->dirp, *tc->size);
	if (TST_RET != -1) {
		tst_res(TFAIL, "getdents() returned %ld", TST_RET);
		return;
	}
	if (TST_ERR == tc->exp_errno) {
		tst_res(TPASS | TTERRNO, "getdents failed as expected");
	} else if (errno == ENOSYS) {
		tst_res(TCONF, "syscall not implemented");
	} else {
		tst_res(TFAIL | TTERRNO, "getdents failed unexpectedly");
	}
}

void main(){
	setup();
	cleanup();
}
