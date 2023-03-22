#include "incl.h"
#define TEST_FILE "test_file"
#define MODE 0777

int fd;

char buf[100];

aio_context_t ctx;

struct iocb iocb;

struct iocb *iocbs[] = {&iocb};

struct tcase {
	aio_context_t *ctx;
	long nr;
	struct iocb **iocbs;
	const char *desc;
} tc[] = {
	{&ctx, 1, iocbs, "returns the number of iocbs submitted"},
	{&ctx, 0, NULL, "returns 0 if nr is zero"},
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
	fd = open(TEST_FILE, O_RDONLY | O_CREAT, MODE);
	io_prep_option(&iocb, fd, buf, 0, 0, IOCB_CMD_PREAD);
}

void cleanup(void)
{
	if (fd > 0)
		close(fd);
	if (tst_syscall(__NR_io_destroy, ctx))
		tst_brk(TBROK | TERRNO, "io_destroy() failed");
}

void run(unsigned int i)
{
	struct io_event evbuf;
	struct timespec timeout = { .tv_sec = 1 };
	long j;
tst_syscall(__NR_io_submit, *tc[i].ctx, tc[i].nr, tc[i].iocbs);
	if (TST_RET == tc[i].nr)
		tst_res(TPASS, "io_submit() %s", tc[i].desc);
	else
		tst_res(TFAIL | TTERRNO, "io_submit() returns %ld, expected %ld", TST_RET, tc[i].nr);
	for (j = 0; j < TST_RET; j++) {
		tst_syscall(__NR_io_getevents, *tc[i].ctx, 1, 1, &evbuf,
			&timeout);
	}
}

void main(){
	setup();
	cleanup();
}
