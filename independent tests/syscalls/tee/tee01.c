#include "incl.h"
#define _GNU_SOURCE
#define TEST_BLOCK_SIZE 1024
#define TESTFILE1 "tee_test_file_1"
#define TESTFILE2 "tee_test_file_2"

int fd_in, fd_out;

char buffer[TEST_BLOCK_SIZE];

void check_file(void)
{
	int i;
	char teebuffer[TEST_BLOCK_SIZE];
	fd_out = open(TESTFILE2, O_RDONLY);
	read(1, fd_out, teebuffer, TEST_BLOCK_SIZE);
	for (i = 0; i < TEST_BLOCK_SIZE; i++) {
		if (buffer[i] != teebuffer[i])
			break;
	}
	if (i < TEST_BLOCK_SIZE)
		tst_res(TFAIL, "Wrong data read from the buffer at %i", i);
	else
		tst_res(TPASS, "Written data has been read back correctly");
	close(fd_out);
}

void tee_test(void)
{
	int pipe1[2];
	int pipe2[2];
	int ret = 0;
	fd_in = open(TESTFILE1, O_RDONLY);
	fd_out = open(TESTFILE2, O_WRONLY | O_CREAT | O_TRUNC, 0777);
	pipe(pipe1);
	pipe(pipe2);
	ret = splice(fd_in, NULL, pipe1[1], NULL, TEST_BLOCK_SIZE, 0);
	if (ret < 0)
		tst_brk(TBROK | TERRNO, "splice(fd_in, pipe1) failed");
	ret = tee(pipe1[0], pipe2[1], TEST_BLOCK_SIZE, SPLICE_F_NONBLOCK);
	if (ret < 0)
		tst_brk(TBROK | TERRNO, "tee() failed");
	ret = splice(pipe2[0], NULL, fd_out, NULL, TEST_BLOCK_SIZE, 0);
	if (ret < 0)
		tst_brk(TBROK | TERRNO, "splice(pipe2, fd_out) failed");
	close(pipe2[0]);
	close(pipe2[1]);
	close(pipe1[0]);
	close(pipe1[1]);
	close(fd_out);
	close(fd_in);
	check_file();
}

void setup(void)
{
	int i;
	for (i = 0; i < TEST_BLOCK_SIZE; i++)
		buffer[i] = i & 0xff;
	fd_in = open(TESTFILE1, O_WRONLY | O_CREAT | O_TRUNC, 0777);
	write(1, fd_in, buffer, TEST_BLOCK_SIZE);
	close(fd_in);
}

void cleanup(void)
{
	if (fd_in > 0)
		close(fd_in);
	if (fd_out > 0)
		close(fd_out);
}

void main(){
	setup();
	cleanup();
}
