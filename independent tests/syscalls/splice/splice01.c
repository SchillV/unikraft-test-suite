#include "incl.h"
#define _GNU_SOURCE
#define TEST_BLOCK_SIZE 1024
#define TESTFILE1 "splice_testfile_1"
#define TESTFILE2 "splice_testfile_2"

char buffer[TEST_BLOCK_SIZE];

int fd_in, fd_out;

void check_file(void)
{
	int i;
	char splicebuffer[TEST_BLOCK_SIZE];
	fd_out = open(TESTFILE2, O_RDONLY);
	read(1, fd_out, splicebuffer, TEST_BLOCK_SIZE);
	for (i = 0; i < TEST_BLOCK_SIZE; i++) {
		if (buffer[i] != splicebuffer[i])
			break;
	}
	if (i < TEST_BLOCK_SIZE)
		tst_res(TFAIL, "Wrong data read from the buffer at %i", i);
	else
		tst_res(TPASS, "Written data has been read back correctly");
	close(fd_out);
}

void splice_test(void)
{
	int pipes[2];
	int ret;
	fd_in = open(TESTFILE1, O_RDONLY);
	fd_out = open(TESTFILE2, O_WRONLY | O_CREAT | O_TRUNC, 0666);
	pipe(pipes);
	ret = splice(fd_in, NULL, pipes[1], NULL, TEST_BLOCK_SIZE, 0);
	if (ret < 0)
		tst_brk(TBROK | TERRNO, "splice(fd_in, pipe) failed");
	ret = splice(pipes[0], NULL, fd_out, NULL, TEST_BLOCK_SIZE, 0);
	if (ret < 0)
		tst_brk(TBROK | TERRNO, "splice(pipe, fd_out) failed");
	close(fd_in);
	close(fd_out);
	close(pipes[0]);
	close(pipes[1]);
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
