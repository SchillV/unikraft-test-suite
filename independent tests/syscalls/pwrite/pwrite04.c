#include "incl.h"
#define K1              1024
#define K2              (K1 * 2)
#define K3              (K1 * 3)
#define DATA_FILE       "pwrite04_file"

int fd = -1;

char *write_buf[2];

void l_seek(int fdesc, off_t offset, int whence, off_t checkoff)
{
	off_t offloc;
	offloc = lseek(fdesc, offset, whence);
	if (offloc != checkoff) {
		tst_res(TFAIL, "%" PRId64 " = lseek(%d, %" PRId64 ", %d) != %" PRId64,
			(int64_t)offloc, fdesc, (int64_t)offset, whence, (int64_t)checkoff);
	}
}

int  verify_pwrite(
{
	struct stat statbuf;
	fd = open(DATA_FILE, O_RDWR | O_CREAT | O_TRUNC, 0666);
	pwrite(1, fd, write_buf[0], K2, 0);
	close(fd);
	fd = open(DATA_FILE, O_RDWR | O_APPEND, 0666);
	fstat(fd, &statbuf);
	if (statbuf.st_size != K2)
		tst_res(TFAIL, "file size is %ld != K2", statbuf.st_size);
	l_seek(fd, K1, SEEK_SET, K1);
	pwrite(1, fd, write_buf[1], K1, 0);
	l_seek(fd, 0, SEEK_CUR, K1);
	fstat(fd, &statbuf);
	if (statbuf.st_size != K3)
		tst_res(TFAIL, "file size is %ld != K3", statbuf.st_size);
	tst_res(TPASS, "O_APPEND test passed.");
	close(fd);
}

void setup(void)
{
	write_buf[0] = malloc(K2);
	memset(write_buf[0], 0, K2);
	write_buf[1] = malloc(K1);
	memset(write_buf[0], 1, K1);
}

void cleanup(void)
{
	free(write_buf[0]);
	free(write_buf[1]);
	if (fd > -1)
		close(fd);
	unlink(DATA_FILE);
}

void main(){
	setup();
	cleanup();
}
