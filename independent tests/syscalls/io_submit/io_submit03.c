#include "incl.h"
#define RDONLY_FILE "rdonly_file"
#define WRONLY_FILE "wronly_file"
#define MODE 0777

char buf[100];

aio_context_t ctx;

aio_context_t invalid_ctx;

struct iocb iocb;

struct iocb *iocbs[] = {&iocb};

struct iocb inv_fd_iocb;

struct iocb *inv_fd_iocbs[] = {&inv_fd_iocb};

int rdonly_fd;

struct iocb rdonly_fd_iocb;

struct iocb *rdonly_fd_iocbs[] = {&rdonly_fd_iocb};

int wronly_fd;

struct iocb wronly_fd_iocb;

struct iocb *wronly_fd_iocbs[] = {&wronly_fd_iocb};

struct iocb *zero_iocbs[1];

struct tcase {
	aio_context_t *ctx;
	long nr;
	struct iocb **iocbs;
	int exp_errno;
	const char *desc;
} tc[] = {
	{&invalid_ctx, 1, iocbs, EINVAL, "invalid ctx"},
	{&ctx, -1, iocbs, EINVAL, "invalid nr"},
	{&ctx, 1, (void*)-1, EFAULT, "invalid iocbpp pointer"},
	{&ctx, 1, zero_iocbs, EFAULT, "NULL iocb pointers"},
	{&ctx, 1, inv_fd_iocbs, EBADF, "invalid fd"},
	{&ctx, 1, rdonly_fd_iocbs, EBADF, "readonly fd for write"},
	{&ctx, 1, wronly_fd_iocbs, EBADF, "writeonly fd for read"},
};

inline void io_prep_option(struct iocb *cb, int fd, void *buf,
			size_t count, long long offset, unsigned opcode)
{
	memset(cb, 0, sizeof(*cb));
	cb->aio_fildes = fd;
	cb->aio_lio_opcode = opcode;
	cb->aio_buf = (uint64_t)buf;
	cb->aio_offset = offset;
	cb->aio_nbytes = count;
}

void setup(void)
{
	TST_EXP_PASS_SILENT(tst_syscall(__NR_io_setup, 1, &ctx));
	io_prep_option(&inv_fd_iocb, -1, buf, sizeof(buf), 0, IOCB_CMD_PREAD);
	rdonly_fd = open(RDONLY_FILE, O_RDONLY | O_CREAT, MODE);
	io_prep_option(&rdonly_fd_iocb, rdonly_fd, buf, sizeof(buf), 0, IOCB_CMD_PWRITE);
	wronly_fd = open(WRONLY_FILE, O_WRONLY | O_CREAT, MODE);
	io_prep_option(&wronly_fd_iocb, wronly_fd, buf, sizeof(buf), 0, IOCB_CMD_PREAD);
}

void cleanup(void)
{
	if (rdonly_fd > 0)
		close(rdonly_fd);
	if (wronly_fd > 0)
		close(wronly_fd);
	if (tst_syscall(__NR_io_destroy, ctx))
		tst_brk(TBROK | TERRNO, "io_destroy() failed");
}

void run(unsigned int i)
{
	TST_EXP_FAIL2(tst_syscall(__NR_io_submit, *tc[i].ctx, tc[i].nr, tc[i].iocbs),
		     tc[i].exp_errno, "io_submit() with %s", tc[i].desc);
}

void main(){
	setup();
	cleanup();
}
