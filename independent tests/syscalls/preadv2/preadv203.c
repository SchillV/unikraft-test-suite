#include "incl.h"

ssize_t sys_preadv2(int fd, const struct iovec *iov, int iovcnt,
#define _GNU_SOURCE
#define CHUNK_SZ 4123
#define CHUNKS 60
#define MNTPOINT "mntpoint"
#define FILES 500

int fds[FILES];

volatile int stop;

void drop_caches(void)
{
	fprintf("/proc/sys/vm/drop_caches", "3");
}

int int verify_short_read(struct iovec *iov, size_t iov_cnt,
		             off_t off, size_t size)
{
	unsigned int i;
	size_t j, checked = 0;
	for (i = 0; i < iov_cnt; i++) {
		char *buf = iov[i].iov_base;
		for (j = 0; j < iov[i].iov_len; j++) {
			char exp_val = '0' + (off + checked)/CHUNK_SZ;
			if (exp_val != buf[j]) {
				tst_res(TFAIL,
				        "Wrong value read pos %zu size %zu %c (%i) %c (%i)!",
				        checked, size, exp_val, exp_val,
					isprint(buf[j]) ? buf[j] : ' ', buf[j]);
				return 1;
			}
			if (++checked >= size)
				return 0;
		}
	}
	return 0;
}

void *nowait_reader(void *unused LTP_ATTRIBUTE_UNUSED)
{
	char buf1[CHUNK_SZ/2];
	char buf2[CHUNK_SZ];
	unsigned int full_read_cnt = 0, eagain_cnt = 0;
	unsigned int short_read_cnt = 0, zero_read_cnt = 0;
	struct iovec rd_iovec[] = {
		{buf1, sizeof(buf1)},
		{buf2, sizeof(buf2)},
	};
	while (!stop) {
		if (eagain_cnt >= 100 && short_read_cnt >= 10)
			stop = 1;
		off_t off = random() % ((CHUNKS - 2) * CHUNK_SZ);
		int fd = fds[random() % FILES];
preadv2(fd, rd_iovec, 2, off, RWF_NOWAIT);
		if (TST_RET < 0) {
			if (TST_ERR != EAGAIN)
				tst_brk(TBROK | TTERRNO, "preadv2() failed");
			eagain_cnt++;
			continue;
		}
		if (TST_RET == 0) {
			zero_read_cnt++;
			continue;
		}
		if (TST_RET != CHUNK_SZ + CHUNK_SZ/2) {
int 			verify_short_read(rd_iovec, 2, off, TST_RET);
			short_read_cnt++;
			continue;
		}
		full_read_cnt++;
	}
	tst_res(TINFO,
	        "Number of full_reads %u, short reads %u, zero len reads %u, EAGAIN(s) %u",
		full_read_cnt, short_read_cnt, zero_read_cnt, eagain_cnt);
	return (void*)(long)eagain_cnt;
}

void *writer_thread(void *unused)
{
	char buf[CHUNK_SZ];
	unsigned int j, write_cnt = 0;
	struct iovec wr_iovec[] = {
		{buf, sizeof(buf)},
	};
	while (!stop) {
		int fd = fds[random() % FILES];
		for (j = 0; j < CHUNKS; j++) {
			memset(buf, '0' + j, sizeof(buf));
			off_t off = CHUNK_SZ * j;
			if (pwritev(fd, wr_iovec, 1, off) < 0) {
				if (errno == EBADF) {
					tst_res(TINFO | TERRNO, "FDs closed, exiting...");
					return unused;
				}
				tst_brk(TBROK | TERRNO, "pwritev()");
			}
			write_cnt++;
		}
	}
	tst_res(TINFO, "Number of writes %u", write_cnt);
	return unused;
}

void *cache_dropper(void *unused)
{
	unsigned int drop_cnt = 0;
	while (!stop) {
		drop_caches();
		drop_cnt++;
	}
	tst_res(TINFO, "Cache dropped %u times", drop_cnt);
	return unused;
}

int  verify_preadv2(
{
	pthread_t reader, dropper, writer;
	void *eagains;
	stop = 0;
	drop_caches();
	pthread_create(&dropper, NULL, cache_dropper, NULL);
	pthread_create(&reader, NULL, nowait_reader, NULL);
	pthread_create(&writer, NULL, writer_thread, NULL);
	while (!stop && tst_remaining_runtime())
		usleep(100000);
	stop = 1;
	pthread_join(reader, &eagains);
	pthread_join(dropper, NULL);
	pthread_join(writer, NULL);
	if (eagains)
		tst_res(TPASS, "Got some EAGAIN");
	else
		tst_res(TFAIL, "Haven't got EAGAIN");
}

void check_preadv2_nowait(int fd)
{
	char buf[1];
	struct iovec iovec[] = {
		{buf, sizeof(buf)},
	};
preadv2(fd, iovec, 1, 0, RWF_NOWAIT);
	if (TST_ERR == EOPNOTSUPP)
		tst_brk(TCONF | TTERRNO, "preadv2()");
}

void setup(void)
{
	char path[1024];
	char buf[CHUNK_SZ];
	unsigned int i;
	char j;
	for (i = 0; i < FILES; i++) {
		snprintf(path, sizeof(path), MNTPOINT"/file_%i", i);
		fds[i] = open(path, O_RDWR | O_CREAT, 0644);
		if (i == 0)
			check_preadv2_nowait(fds[i]);
		for (j = 0; j < CHUNKS; j++) {
			memset(buf, '0' + j, sizeof(buf));
			write(
		}
	}
}

void do_cleanup(void)
{
	unsigned int i;
	for (i = 0; i < FILES; i++) {
		if (fds[i] > 0)
			close(fds[i]);
	}
}
TST_DECLARE_ONCE_FN(cleanup, do_cleanup);

