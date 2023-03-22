#include "incl.h"
#define TESTFILE1	"testfile1"
#define TESTFILE2	"testfile2"

int sock_fd, read_fd, fd;

int bad_fd = -1;

struct tcase {
	int *fd;
	off_t length;
	int exp_errno;
} tcases[] = {
	{&sock_fd, 4, EINVAL},
	{&read_fd, 4, EINVAL},
	{&fd, -1, EINVAL},
	{&bad_fd, 4, EBADF},
};

int  verify_ftruncate(unsigned int n)
{
	struct tcase *tc = &tcases[n];
ftruncate(*tc->fd, tc->length);
	if (TST_RET != -1) {
		tst_res(TFAIL, "ftruncate() succeeded unexpectedly and got %ld",
			TST_RET);
		return;
	}
	if (TST_ERR == tc->exp_errno) {
		tst_res(TPASS | TTERRNO, "ftruncate() failed expectedly");
	} else {
		tst_res(TFAIL | TTERRNO,
			"ftruncate() failed unexpectedly, got %s, expected",
			tst_strerrno(tc->exp_errno));
	}
}

void setup(void)
{
	sock_fd = socket(PF_INET, SOCK_STREAM, 0);
	read_fd = open(TESTFILE1, O_RDONLY | O_CREAT, 0644);
	fd = open(TESTFILE2, O_RDWR | O_CREAT, 0644);
}

void cleanup(void)
{
	if (sock_fd > 0)
		close(sock_fd);
	if (read_fd > 0)
		close(read_fd);
	if (fd > 0)
		close(fd);
}

void main(){
	setup();
	cleanup();
}
