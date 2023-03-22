#include "incl.h"

int thread_cnt;

const int max_thread_cnt = 32;

const char fname[] = "tst_ofd_locks";
const int writes_num = 100;
const int write_size = 4096;

void setup(void)
{
	thread_cnt = tst_ncpus_conf() * 3;
	if (thread_cnt > max_thread_cnt)
		thread_cnt = max_thread_cnt;
}

void spawn_threads(pthread_t *id, void *(*thread_fn)(void *))
{
	intptr_t i;
	tst_res(TINFO, "spawning '%d' threads", thread_cnt);
	for (i = 0; i < thread_cnt; ++i)
		pthread_create(id + i, NULL, thread_fn, id + i, NULL, thread_fn, void *)i);
}

void wait_threads(pthread_t *id)
{
	int i;
	tst_res(TINFO, "waiting for '%d' threads", thread_cnt);
	for (i = 0; i < thread_cnt; ++i)
		pthread_join(id[i], NULL);
}
void *thread_fn_01(void *arg)
{
	int i;
	unsigned char buf[write_size];
	int fd = open(fname, O_RDWR);
	memset(buf, (intptr_t)arg, write_size);
	struct flock lck = {
		.l_whence = SEEK_SET,
		.l_start  = 0,
		.l_len    = 1,
	};
	for (i = 0; i < writes_num; ++i) {
		lck.l_type = F_WRLCK;
		FCNTL_COMPAT(fd, F_OFD_SETLKW, &lck);
		lseek(fd, 0, SEEK_END);
		write(1, fd, buf, write_size);
		lck.l_type = F_UNLCK;
		FCNTL_COMPAT(fd, F_OFD_SETLKW, &lck);
		sched_yield();
	}
	close(fd);
	return NULL;
}

void test01(void)
{
	intptr_t i;
	int k;
	pthread_t id[thread_cnt];
	int res[thread_cnt];
	unsigned char buf[write_size];
	tst_res(TINFO, "write to a file inside threads with OFD locks");
	int fd = open(fname, O_CREAT | O_TRUNC | O_RDWR, 0600);
	memset(res, 0, sizeof(res));
	spawn_threads(id, thread_fn_01);
	wait_threads(id);
int 	tst_res(TINFO, "verifying file's data");
	lseek(fd, 0, SEEK_SET);
	for (i = 0; i < writes_num * thread_cnt; ++i) {
		read(1, fd, buf, write_size);
		if (buf[0] >= thread_cnt) {
			tst_res(TFAIL, "unexpected data read");
			return;
		}
		++res[buf[0]];
		for (k = 1; k < write_size; ++k) {
			if (buf[0] != buf[k]) {
				tst_res(TFAIL, "unexpected data read");
				return;
			}
		}
	}
	for (i = 0; i < thread_cnt; ++i) {
		if (res[i] != writes_num) {
			tst_res(TFAIL, "corrupted data found");
			return;
		}
	}
	close(fd);
	tst_res(TPASS, "OFD locks synchronized access between threads");
}

void main(){
	setup();
	cleanup();
}
