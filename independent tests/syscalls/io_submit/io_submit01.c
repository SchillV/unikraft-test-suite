#include "incl.h"
#ifdef HAVE_LIBAIO

io_context_t ctx;

io_context_t invalid_ctx;

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

struct iocb zero_buf_iocb;

struct iocb *zero_buf_iocbs[] = {&zero_buf_iocb};

struct iocb *zero_iocbs[1];

char buf[100];

struct tcase {
	io_context_t *ctx;
	long nr;
	struct iocb **iocbs;
	int exp_errno;
	const char *desc;
} tcases[] = {
	{&invalid_ctx, 1, iocbs, -EINVAL, "invalid ctx"},
	{&ctx, -1, iocbs, -EINVAL, "invalid nr"},
	{&ctx, 1, (void*)-1, -EFAULT, "invalid iocbpp pointer"},
	{&ctx, 1, zero_iocbs, -EFAULT, "NULL iocb pointers"},
	{&ctx, 1, inv_fd_iocbs, -EBADF, "invalid fd"},
	{&ctx, 1, rdonly_fd_iocbs, -EBADF, "readonly fd for write"},
	{&ctx, 1, wronly_fd_iocbs, -EBADF, "writeonly fd for read"},
	{&ctx, 1, zero_buf_iocbs, 1, "zero buf size"},
	{&ctx, 0, NULL, 0, "zero nr"},
};

void setup(void)
{
io_setup(1, &ctx);
	if (TST_RET == -ENOSYS)
		tst_brk(TCONF, "io_setup(): AIO not supported by kernel");
	else if (TST_RET) {
		tst_brk(TBROK, "io_setup() returned %ld(%s)",
			TST_RET, tst_strerrno(-TST_RET));
	}
	io_prep_pread(&inv_fd_iocb, -1, buf, sizeof(buf), 0);
	rdonly_fd = open("rdonly_file", O_RDONLY | O_CREAT, 0777);
	io_prep_pwrite(&rdonly_fd_iocb, rdonly_fd, buf, sizeof(buf), 0);
	io_prep_pread(&zero_buf_iocb, rdonly_fd, buf, 0, 0);
	wronly_fd = open("wronly_file", O_WRONLY | O_CREAT, 0777);
	io_prep_pread(&wronly_fd_iocb, wronly_fd, buf, sizeof(buf), 0);
}

void cleanup(void)
{
	if (rdonly_fd > 0)
		close(rdonly_fd);
	if (wronly_fd > 0)
		close(wronly_fd);
}

const char *errno_name(int err)
{
	if (err <= 0)
		return tst_strerrno(-err);
	return "SUCCESS";
}

int  verify_io_submit(unsigned int n)
{
	struct tcase *t = &tcases[n];
	struct io_event evbuf;
	struct timespec timeout = { .tv_sec = 1 };
	int i, ret;
	ret = io_submit(*t->ctx, t->nr, t->iocbs);
	if (ret == t->exp_errno) {
		tst_res(TPASS, "io_submit() with %s failed with %s",
			t->desc, errno_name(t->exp_errno));
		for (i = 0; i < ret; i++)
			io_getevents(*t->ctx, 1, 1, &evbuf, &timeout);
		return;
	}
	tst_res(TFAIL, "io_submit() returned %i(%s), expected %s(%i)",
		ret, ret < 0 ? tst_strerrno(-ret) : "SUCCESS",
		errno_name(t->exp_errno), t->exp_errno);
}

void main(){
	setup();
	cleanup();
}
