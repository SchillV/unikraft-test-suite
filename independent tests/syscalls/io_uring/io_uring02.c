#include "incl.h"
#define CHROOT_DIR "test_root"
#define SOCK_NAME "sock"
#define SPAM_MARK 0xfa7
#define BEEF_MARK 0xbeef

struct sockaddr_un addr;

int sendsock = -1, recvsock = -1, sockpair[2] = {-1, -1};

struct io_uring_params params;

struct tst_io_uring uring = { .fd = -1 };

char buf[16];

struct iovec iov = {
	.iov_base = buf,
	.iov_len = sizeof(buf)
};

struct msghdr spam_header = {
	.msg_name = NULL,
	.msg_namelen = 0,
	.msg_iov = &iov,
	.msg_iovlen = 1
};

struct msghdr beef_header = {
	.msg_name = &addr,
	.msg_namelen = sizeof(addr),
	.msg_iov = &iov,
	.msg_iovlen = 1
};

void setup(void)
{
	char *tmpdir = tst_get_tmpdir();
	int ret;
	addr.sun_family = AF_UNIX;
	ret = snprintf(addr.sun_path, sizeof(addr.sun_path), "%s/%s", tmpdir,
		SOCK_NAME);
	free(tmpdir);
	if (ret >= (int)sizeof(addr.sun_path))
		tst_brk(TBROK, "Tempdir path is too long");
	io_uring_setup_supported_by_kernel();
	sendsock = socket(AF_UNIX, SOCK_DGRAM, 0);
	recvsock = socket(AF_UNIX, SOCK_DGRAM, 0);
	bind(recvsock, recvsock, struct sockaddr *)&addr, sizeofaddr));
	mkdir(CHROOT_DIR, 0755);
	chroot(CHROOT_DIR);
}

void drain_fallback(void)
{
	uint32_t i, count, tail;
	int beef_found = 0;
	struct io_uring_sqe *sqe_ptr = uring.sqr_entries;
	const struct io_uring_cqe *cqe_ptr;
	socketpair(AF_UNIX, SOCK_DGRAM, 0, sockpair);
	setsockopt_int(sockpair[0], SOL_SOCKET, SO_SNDBUF,
		32+sizeof(buf));
	fcntl(sockpair[0], F_SETFL, O_NONBLOCK);
	for (i = 0, tail = *uring.sqr_tail; i < 255; i++, tail++, sqe_ptr++) {
		memset(sqe_ptr, 0, sizeof(*sqe_ptr));
		sqe_ptr->opcode = IORING_OP_SENDMSG;
		sqe_ptr->flags = IOSQE_IO_DRAIN;
		sqe_ptr->fd = sockpair[0];
		sqe_ptr->addr = (__u64)&spam_header;
		sqe_ptr->user_data = SPAM_MARK;
		uring.sqr_array[tail & *uring.sqr_mask] = i;
	}
	memset(sqe_ptr, 0, sizeof(*sqe_ptr));
	sqe_ptr->opcode = IORING_OP_SENDMSG;
	sqe_ptr->flags = IOSQE_IO_DRAIN;
	sqe_ptr->fd = sendsock;
	sqe_ptr->addr = (__u64)&beef_header;
	sqe_ptr->user_data = BEEF_MARK;
	uring.sqr_array[tail & *uring.sqr_mask] = i;
	count = ++i;
	tail++;
	__atomic_store(uring.sqr_tail, &tail, __ATOMIC_RELEASE);
	io_uring_enter(1, uring.fd, count, count, IORING_ENTER_GETEVENTS,
		NULL);
	__atomic_load(uring.cqr_tail, &tail, __ATOMIC_ACQUIRE);
	for (i = *uring.cqr_head; i != tail; i++, count--) {
		cqe_ptr = uring.cqr_entries + (i & *uring.cqr_mask);
		TST_ERR = -cqe_ptr->res;
		if (cqe_ptr->user_data == SPAM_MARK) {
			if (cqe_ptr->res >= 0 || cqe_ptr->res == -EAGAIN)
				continue;
			tst_res(TFAIL | TTERRNO,
				"Spam request failed unexpectedly");
			continue;
		}
		if (cqe_ptr->user_data != BEEF_MARK) {
			tst_res(TFAIL, "Unexpected entry in completion queue");
			count++;
			continue;
		}
		beef_found = 1;
		if (cqe_ptr->res >= 0) {
			tst_res(TFAIL, "Write outside chroot succeeded.");
		} else if (cqe_ptr->res != -ENOENT) {
			tst_res(TFAIL | TTERRNO,
				"Write outside chroot failed unexpectedly");
		} else {
			tst_res(TPASS | TTERRNO,
				"Write outside chroot failed as expected");
		}
	}
	__atomic_store(uring.cqr_head, &i, __ATOMIC_RELEASE);
	if (!beef_found)
		tst_res(TFAIL, "Write outside chroot result not found");
	if (count)
		tst_res(TFAIL, "Wrong number of entries in completion queue");
	close(sockpair[0]);
	close(sockpair[1]);
}

void check_result(void)
{
	const struct io_uring_cqe *cqe_ptr;
	cqe_ptr = uring.cqr_entries + (*uring.cqr_head & *uring.cqr_mask);
	++*uring.cqr_head;
	TST_ERR = -cqe_ptr->res;
	if (cqe_ptr->user_data != BEEF_MARK) {
		tst_res(TFAIL, "Unexpected entry in completion queue");
		return;
	}
	if (cqe_ptr->res == -EINVAL) {
		tst_res(TINFO, "IOSQE_ASYNC is not supported, using fallback");
		drain_fallback();
		return;
	}
	tst_res(TINFO, "IOSQE_ASYNC is supported");
	if (cqe_ptr->res >= 0) {
		tst_res(TFAIL, "Write outside chroot succeeded.");
		return;
	}
	if (cqe_ptr->res != -ENOENT) {
		tst_res(TFAIL | TTERRNO,
			"Write outside chroot failed unexpectedly");
		return;
	}
	tst_res(TPASS | TTERRNO, "Write outside chroot failed as expected");
}

void run(void)
{
	uint32_t tail;
	struct io_uring_sqe *sqe_ptr;
	io_uring_init(512, &params, &uring);
	sqe_ptr = uring.sqr_entries;
	tail = *uring.sqr_tail;
	memset(sqe_ptr, 0, sizeof(*sqe_ptr));
	sqe_ptr->opcode = IORING_OP_SENDMSG;
	sqe_ptr->flags = IOSQE_ASYNC;
	sqe_ptr->fd = sendsock;
	sqe_ptr->addr = (__u64)&beef_header;
	sqe_ptr->user_data = BEEF_MARK;
	uring.sqr_array[tail & *uring.sqr_mask] = 0;
	tail++;
	__atomic_store(uring.sqr_tail, &tail, __ATOMIC_RELEASE);
	io_uring_enter(1, uring.fd, 1, 1, IORING_ENTER_GETEVENTS, NULL);
	check_result();
	io_uring_close(&uring);
}

void cleanup(void)
{
	if (uring.fd >= 0)
		io_uring_close(&uring);
	if (sockpair[0] >= 0) {
		close(sockpair[0]);
		close(sockpair[1]);
	}
	if (recvsock >= 0)
		close(recvsock);
	if (sendsock >= 0)
		close(sendsock);
}

void main(){
	setup();
	cleanup();
}
