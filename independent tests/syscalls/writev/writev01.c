#include "incl.h"
#define	CHUNK		64
#define	TESTFILE	"writev_data_file"

int valid_fd;

int invalid_fd = -1;

int pipe_fd[2];

char buf[CHUNK * 4];
struct iovec iovec_badlen[] = {
	{ buf, -1 },
	{ buf + CHUNK, CHUNK },
	{ buf + CHUNK * 2, CHUNK },
};
struct iovec iovec_simple[] = {
	{ buf, CHUNK },
};
struct iovec iovec_zero_null[] = {
	{ buf, CHUNK },
	{ buf + CHUNK, 0 },
	{ NULL, 0 },
	{ NULL, 0 }
};
struct testcase_t {
	const char *desc;
	int *pfd;
	struct iovec (*piovec)[];
	int iovcnt;
	int exp_ret;
	int exp_errno;
} testcases[] = {
	{
		.desc = "invalid iov_len",
		.pfd = &valid_fd,
		.piovec = &iovec_badlen,
		.iovcnt = ARRAY_SIZE(iovec_badlen),
		.exp_ret = -1,
		.exp_errno = EINVAL
	},
	{
		.desc = "invalid fd",
		.pfd = &invalid_fd,
		.piovec = &iovec_simple,
		.iovcnt = ARRAY_SIZE(iovec_simple),
		.exp_ret = -1,
		.exp_errno = EBADF
	},
	{
		.desc = "invalid iovcnt",
		.pfd = &valid_fd,
		.piovec = &iovec_simple,
		.iovcnt = -1,
		.exp_ret = -1,
		.exp_errno = EINVAL
	},
	{
		.desc = "zero iovcnt",
		.pfd = &valid_fd,
		.piovec = &iovec_simple,
		.iovcnt = 0,
		.exp_ret = 0,
	},
	{
		.desc = "NULL and zero length iovec",
		.pfd = &valid_fd,
		.piovec = &iovec_zero_null,
		.iovcnt = ARRAY_SIZE(iovec_zero_null),
		.exp_ret = CHUNK,
	},
	{
		.desc = "write to closed pipe",
		.pfd = &(pipe_fd[1]),
		.piovec = &iovec_simple,
		.iovcnt = ARRAY_SIZE(iovec_simple),
		.exp_ret = -1,
		.exp_errno = EPIPE,
	},
};
void setup(void)
{
	sigset_t block_mask;
	sigemptyset(&block_mask);
	sigaddset(&block_mask, SIGPIPE);
	sigprocmask(SIG_BLOCK, &block_mask, NULL);
	valid_fd = open(TESTFILE, O_RDWR | O_CREAT, 0644);
	pipe(pipe_fd);
	close(pipe_fd[0]);
}

void test_writev(unsigned int i)
{
	struct testcase_t *tcase = &testcases[i];
	int ret;
writev(*(tcase->pfd), *(tcase->piovec), tcase->iovcnt);
	ret = (TST_RET == tcase->exp_ret);
	if (TST_RET < 0 || tcase->exp_ret < 0) {
		ret &= (TST_ERR == tcase->exp_errno);
		tst_res((ret ? TPASS : TFAIL),
			"%s, expected: %d (%s), got: %ld (%s)", tcase->desc,
			tcase->exp_ret, tst_strerrno(tcase->exp_errno),
			TST_RET, tst_strerrno(TST_ERR));
	} else {
		tst_res((ret ? TPASS : TFAIL),
			"%s, expected: %d, got: %ld", tcase->desc,
			tcase->exp_ret, TST_RET);
	}
}

void main(){
	setup();
	cleanup();
}
