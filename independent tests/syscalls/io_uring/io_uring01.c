#include "incl.h"
#define TEST_FILE "test_file"
#define QUEUE_DEPTH 1
#define BLOCK_SZ    1024

struct tcase {
	unsigned int setup_flags;
	unsigned int register_opcode;
	unsigned int enter_flags;
} tcases[] = {
	{0, IORING_REGISTER_BUFFERS, IORING_OP_READ_FIXED},
};
struct io_sq_ring {
	unsigned int *head;
	unsigned int *tail;
	unsigned int *ring_mask;
	unsigned int *ring_entries;
	unsigned int *flags;
	unsigned int *array;
};
struct io_cq_ring {
	unsigned int *head;
	unsigned int *tail;
	unsigned int *ring_mask;
	unsigned int *ring_entries;
	struct io_uring_cqe *cqes;
};
struct submitter {
	int ring_fd;
	struct io_sq_ring sq_ring;
	struct io_uring_sqe *sqes;
	struct io_cq_ring cq_ring;
};

struct submitter sub_ring;

struct submitter *s = &sub_ring;

sigset_t sig;

struct iovec *iov;

void *sptr;

size_t sptr_size;

void *cptr;

size_t cptr_size;

int setup_io_uring_test(struct submitter *s, struct tcase *tc)
{
	struct io_sq_ring *sring = &s->sq_ring;
	struct io_cq_ring *cring = &s->cq_ring;
	struct io_uring_params p;
	memset(&p, 0, sizeof(p));
	p.flags |= tc->setup_flags;
	s->ring_fd = io_uring_setup(QUEUE_DEPTH, &p);
	if (s->ring_fd != -1) {
		tst_res(TPASS, "io_uring_setup() passed");
	} else {
		tst_res(TFAIL | TERRNO, "io_uring_setup() failed");
		return 1;
	}
	sptr_size = p.sq_off.array + p.sq_entries * sizeof(unsigned int);
	sptr = mmap(0, sptr_size,
			PROT_READ | PROT_WRITE,
			MAP_SHARED | MAP_POPULATE,
			s->ring_fd, IORING_OFF_SQ_RING);
	sring->head = sptr + p.sq_off.head;
	sring->tail = sptr + p.sq_off.tail;
	sring->ring_mask = sptr + p.sq_off.ring_mask;
	sring->ring_entries = sptr + p.sq_off.ring_entries;
	sring->flags = sptr + p.sq_off.flags;
	sring->array = sptr + p.sq_off.array;
	s->sqes = mmap(0, p.sq_entries *
			sizeof(struct io_uring_sqe),
			PROT_READ | PROT_WRITE,
			MAP_SHARED | MAP_POPULATE,
			s->ring_fd, IORING_OFF_SQES);
	cptr_size = p.cq_off.cqes + p.cq_entries * sizeof(struct io_uring_cqe);
	cptr = mmap(0, cptr_size,
			PROT_READ | PROT_WRITE,
			MAP_SHARED | MAP_POPULATE,
			s->ring_fd, IORING_OFF_CQ_RING);
	cring->head = cptr + p.cq_off.head;
	cring->tail = cptr + p.cq_off.tail;
	cring->ring_mask = cptr + p.cq_off.ring_mask;
	cring->ring_entries = cptr + p.cq_off.ring_entries;
	cring->cqes = cptr + p.cq_off.cqes;
	return 0;
}

void check_buffer(char *buffer, size_t len)
{
	size_t i;
	for (i = 0; i < len; i++) {
		if (buffer[i] != 'a') {
			tst_res(TFAIL, "Wrong data at offset %zu", i);
			break;
		}
	}
	if (i == len)
		tst_res(TPASS, "Buffer filled in correctly");
}

void drain_uring_cq(struct submitter *s, unsigned int exp_events)
{
	struct io_cq_ring *cring = &s->cq_ring;
	unsigned int head = *cring->head;
	unsigned int events = 0;
	for (head = *cring->head; head != *cring->tail; head++) {
		struct io_uring_cqe *cqe = &cring->cqes[head & *s->cq_ring.ring_mask];
		events++;
		if (cqe->res < 0) {
			tst_res(TFAIL, "CQE result %s", tst_strerrno(-cqe->res));
		} else {
			struct iovec *iovecs = (void*)cqe->user_data;
			if (cqe->res == BLOCK_SZ)
				tst_res(TPASS, "CQE result %i", cqe->res);
			else
				tst_res(TFAIL, "CQE result %i expected %i", cqe->res, BLOCK_SZ);
			check_buffer(iovecs[0].iov_base, cqe->res);
		}
	}
	if (exp_events == events) {
		tst_res(TPASS, "Got %u completion events", events);
		return;
	}
	tst_res(TFAIL, "Got %u completion events expected %u",
	        events, exp_events);
}

int submit_to_uring_sq(struct submitter *s, struct tcase *tc)
{
	unsigned int index = 0, tail = 0, next_tail = 0;
	struct io_sq_ring *sring = &s->sq_ring;
	struct io_uring_sqe *sqe;
	int ret;
	memset(iov->iov_base, 0, iov->iov_len);
	ret = io_uring_register(s->ring_fd, tc->register_opcode,
				iov, QUEUE_DEPTH);
	if (ret == 0) {
		tst_res(TPASS, "io_uring_register() passed");
	} else {
		tst_res(TFAIL | TERRNO, "io_uring_register() failed");
		return 1;
	}
	int fd = open(TEST_FILE, O_RDONLY);
	tail = *sring->tail;
	next_tail = tail + 1;
	index = tail & *s->sq_ring.ring_mask;
	sqe = &s->sqes[index];
	sqe->flags = 0;
	sqe->fd = fd;
	sqe->opcode = tc->enter_flags;
	sqe->addr = (unsigned long)iov->iov_base;
	sqe->len = BLOCK_SZ;
	sqe->off = 0;
	sqe->user_data = (unsigned long long)iov;
	sring->array[index] = index;
	tail = next_tail;
	if (*sring->tail != tail)
		*sring->tail = tail;
	ret = io_uring_enter(s->ring_fd, 1, 1, IORING_ENTER_GETEVENTS, &sig);
	if (ret == 1) {
		tst_res(TPASS, "io_uring_enter() waited for 1 event");
	} else {
		tst_res(TFAIL | TERRNO, "io_uring_enter() returned %i", ret);
		close(fd);
		return 1;
	}
	close(fd);
	return 0;
}

void cleanup_io_uring_test(void)
{
	io_uring_register(s->ring_fd, IORING_UNREGISTER_BUFFERS,
			  NULL, QUEUE_DEPTH);
	munmap(s->sqes, sizeofs->sqes, sizeofstruct io_uring_sqe));
	munmap(cptr, cptr_size);
	munmap(sptr, sptr_size);
	close(s->ring_fd);
}

void run(unsigned int n)
{
	struct tcase *tc = &tcases[n];
	if (setup_io_uring_test(s, tc))
		return;
	if (!submit_to_uring_sq(s, tc))
		drain_uring_cq(s, 1);
	cleanup_io_uring_test();
}

void setup(void)
{
	io_uring_setup_supported_by_kernel();
	tst_fill_file(TEST_FILE, 'a', 1024, 1);
}

void main(){
	setup();
	cleanup();
}
