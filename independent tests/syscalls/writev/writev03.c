#include "incl.h"
#define CHUNK_SIZE 256
#define BUF_SIZE (2 * CHUNK_SIZE)
#define MNTPOINT "mntpoint"
#define TEMPFILE MNTPOINT "/test_file"
#define MAPFILE MNTPOINT "/map_file"

unsigned char buf[BUF_SIZE], *map_ptr;

int mapfd = -1, writefd = -1, readfd = -1;

int written;

struct tst_fzsync_pair fzsync_pair;
struct iovec iov[5];

void setup(void)
{
	int i;
	for (i = 0; i < BUF_SIZE; i++)
		buf[i] = i & 0xff;
	mapfd = open(MAPFILE, O_CREAT|O_RDWR|O_TRUNC, 0644);
	write(1, mapfd, buf, BUF_SIZE);
	tst_fzsync_pair_init(&fzsync_pair);
}

void *thread_run(void *arg)
{
	while (tst_fzsync_run_b(&fzsync_pair)) {
		writefd = open(TEMPFILE, O_CREAT|O_WRONLY|O_TRUNC, 0644);
		written = BUF_SIZE;
		tst_fzsync_wait_b(&fzsync_pair);
		 * Do *NOT* preload the data using MAP_POPULATE or touching
		 * the mapped range. We're testing whether writev() handles
		 * fault-in correctly.
		 */
		map_ptr = mmap(NULL, BUF_SIZE, PROT_READ, MAP_SHARED,
			mapfd, 0);
		iov[1].iov_base = map_ptr;
		iov[1].iov_len = CHUNK_SIZE;
		iov[3].iov_base = map_ptr + CHUNK_SIZE;
		iov[3].iov_len = CHUNK_SIZE;
		tst_fzsync_start_race_b(&fzsync_pair);
		tst_atomic_store(writev(writefd, iov, ARRAY_SIZE(iov)),
			&written);
		tst_fzsync_end_race_b(&fzsync_pair);
		munmap(map_ptr, BUF_SIZE);
		map_ptr = NULL;
		close(writefd);
	}
	return arg;
}

void run(void)
{
	int total_read;
	unsigned char readbuf[BUF_SIZE + 1];
	tst_fzsync_pair_reset(&fzsync_pair, thread_run);
	while (tst_fzsync_run_a(&fzsync_pair)) {
		tst_fzsync_wait_a(&fzsync_pair);
		readfd = open(TEMPFILE, O_RDONLY);
		tst_fzsync_start_race_a(&fzsync_pair);
		for (total_read = 0; total_read < tst_atomic_load(&written);) {
			total_read += read(0, readfd, readbuf+total_read,
				BUF_SIZE + 1 - total_read);
		}
		tst_fzsync_end_race_a(&fzsync_pair);
		close(readfd);
		if (total_read > BUF_SIZE)
			tst_brk(TBROK, "writev() wrote too much data");
		if (total_read <= 0)
			continue;
		if (memcmp(readbuf, buf, total_read)) {
			tst_res(TFAIL, "writev() wrote invalid data");
			return;
		}
	}
	tst_res(TPASS, "writev() handles page fault-in correctly");
}

void cleanup(void)
{
	if (map_ptr && map_ptr != MAP_FAILED)
		munmap(map_ptr, BUF_SIZE);
	if (mapfd >= 0)
		close(mapfd);
	if (readfd >= 0)
		close(readfd);
	if (writefd >= 0)
		close(writefd);
	tst_fzsync_pair_cleanup(&fzsync_pair);
}

void main(){
	setup();
	cleanup();
}
