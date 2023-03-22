#include "incl.h"
#define TEMPFILE        "pread_file"
#define K1              1024
#define K2              (K1 * 2)
#define K3              (K1 * 3)
#define K4              (K1 * 4)
#define NBUFS           4

int fildes;

char *write_buf[NBUFS];

char *read_buf[NBUFS];

void l_seek(int fdesc, off_t offset, int whence, off_t checkoff)
{
	off_t offloc;
	offloc = lseek(fdesc, offset, whence);
	if (offloc != checkoff) {
		tst_res(TFAIL, "return = %" PRId64 ", expected %" PRId64,
			(int64_t) offloc, (int64_t) checkoff);
	}
}

void compare_bufers(void)
{
	int count;
	int err_flg = 0;
	for (count = 0; count < NBUFS; count++) {
		if (memcmp(write_buf[count], read_buf[count], K1) != 0) {
			tst_res(TFAIL, "read/write buffer[%d] data mismatch", count);
			err_flg++;
		}
	}
	if (!err_flg)
		tst_res(TPASS, "Functionality of pread() is correct");
}

int  verify_pread(
{
	pread(1, fildes, read_buf[2], K1, K2);
	l_seek(fildes, 0, SEEK_CUR, K4);
	l_seek(fildes, 0, SEEK_SET, 0);
	pread(1, fildes, read_buf[3], K1, K3);
	l_seek(fildes, 0, SEEK_CUR, 0);
	read(1, fildes, read_buf[0], K1);
	l_seek(fildes, 0, SEEK_CUR, K1);
	pread(1, fildes, read_buf[1], K1, K1);
	l_seek(fildes, 0, SEEK_CUR, K1);
	compare_bufers();
	l_seek(fildes, K4, SEEK_SET, K4);
}

void setup(void)
{
	int count;
	for (count = 0; count < NBUFS; count++) {
		write_buf[count] = malloc(K1);
		read_buf[count] = malloc(K1);
		memset(write_buf[count], count, K1);
	}
	fildes = open(TEMPFILE, O_RDWR | O_CREAT, 0666);
	pwrite(1, fildes, write_buf[0], K1, 0);
	pwrite(1, fildes, write_buf[2], K1, K2);
	pwrite(1, fildes, write_buf[3], K1, K3);
	pwrite(1, fildes, write_buf[1], K1, K1);
	lseek(fildes, K4, SEEK_SET);
}

void cleanup(void)
{
	int count;
	for (count = 0; count < NBUFS; count++) {
		free(write_buf[count]);
		free(read_buf[count]);
	}
	if (fildes > 0)
		close(fildes);
}

void main(){
	setup();
	cleanup();
}
