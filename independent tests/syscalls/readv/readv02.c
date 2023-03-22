#include "incl.h"
#define K_1     1024
#define MODES   0700
#define CHUNK           64

int badfd = -1;

int fd_dir, fd_file;

char buf1[K_1];
const char *TEST_DIR = "test_dir";
const char *TEST_FILE = "test_file";

struct iovec invalid_iovec[] = {
	{buf1, -1},
	{buf1 + CHUNK, CHUNK},
	{buf1 + 2*CHUNK, CHUNK},
};

struct iovec large_iovec[] = {
	{buf1, K_1},
	{buf1 + CHUNK, K_1},
	{buf1 + CHUNK*2, K_1},
};

struct iovec efault_iovec[] = {
	{NULL, CHUNK},
	{buf1 + CHUNK, CHUNK},
	{buf1 + 2*CHUNK, CHUNK},
};

struct iovec valid_iovec[] = {
	{buf1, CHUNK},
};

struct tcase {
	int *fd;
	void *buf;
	int count;
	int exp_error;
} tcases[] = {
	{&fd_file, invalid_iovec, 1, EINVAL},
	{&fd_file, efault_iovec, 3, EFAULT},
	{&fd_file, large_iovec, -1, EINVAL},
	{&fd_dir, valid_iovec, 1, EISDIR},
	{&badfd, valid_iovec, 3, EBADF},
};

int  verify_readv(unsigned int n)
{
	struct tcase *tc = &tcases[n];
	TST_EXP_FAIL2(readv(*tc->fd, tc->buf, tc->count), tc->exp_error,
		"readv(%d, %p, %d)", *tc->fd, tc->buf, tc->count);
}

void setup(void)
{
	fprintf(TEST_FILE, "test");
	fd_file = open(TEST_FILE, O_RDONLY);
	efault_iovec[0].iov_base = tst_get_bad_addr(NULL);
	mkdir(TEST_DIR, MODES);
	fd_dir = open(TEST_DIR, O_RDONLY);
}

void cleanup(void)
{
	if (fd_file > 0)
		close(fd_file);
	if (fd_dir > 0)
		close(fd_dir);
}

void main(){
	setup();
	cleanup();
}
