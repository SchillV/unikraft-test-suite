#include "incl.h"
#define K1              1024
#define K2              (K1 * 2)
#define K3              (K1 * 3)
#define DATA_FILE       "write06_file"

int fd = -1;

char *write_buf[2];

int  verify_write()
{
	off_t off;
	struct stat statbuf;
	fd = open(DATA_FILE, O_RDWR | O_CREAT | O_TRUNC, 0666);
	int ok = -1;
	write(fd, write_buf[0], K2);
	close(fd);
	fd = open(DATA_FILE, O_RDWR | O_APPEND);
	fstat(fd, &statbuf);
	if (statbuf.st_size != K2)
		printf("file size is %ld != K2\n", statbuf.st_size);
	off = lseek(fd, K1, SEEK_SET);
	if (off != K1){
		printf("Failed to seek to K1\n");
		ok = 0;
	}
	write(fd, write_buf[1], K1);
	off = lseek(fd, 0, SEEK_CUR);
	if (off != K3)
		printf("Wrong offset after write %zu expected %u\n", off, K3);
	else{
		printf("Offset is correct after write %zu\n", off);
		ok = 1;
	}
	fstat(fd, &statbuf);
	if (statbuf.st_size != K3)
		printf("Wrong file size after append %zu expected %u\n", statbuf.st_size, K3);
	else{
		printf("Correct file size after append %u\n", K3);
		ok = 1;
	}
	close(fd);
	return ok;
}

void setup(void)
{
	memset(write_buf[0], 0, K2);
	memset(write_buf[1], 1, K1);
}

void cleanup(void)
{
	if (fd != -1)
		close(fd);
	unlink(DATA_FILE);
}

void main(){
	setup();
	int res = verify_write();
	if(res == 1)
		printf("test succeeded\n");
	else if(res == -1)
		printf("test failed\n");
	else
		printf("the test seems to be broken\n");
	cleanup();
}
