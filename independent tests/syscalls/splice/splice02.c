#include "incl.h"
#define _GNU_SOURCE
#define BUFSIZE 512
#define SPLICE_SIZE 1024
#define TEST_FILENAME "splice02-temp"

char *sarg;

int file_size;

int pipe_fd[2];

void setup(void)
{
	if (tst_parse_int(sarg, &file_size, 1, INT_MAX))
		tst_brk(TBROK, "invalid number of writes '%s', use <1,%d>", sarg, INT_MAX);
}

inline int get_letter(int n)
{
	return n % ('z' - 'a' + 1) + 'a';
}

void do_child(void)
{
	int fd;
	size_t page_size, to_check, block, blocks, i, fail = 0;
	struct stat st;
	char *map;
	close(pipe_fd[1]);
	dup2(pipe_fd[0], STDIN_FILENO);
	fd = open(TEST_FILENAME, O_WRONLY | O_CREAT | O_TRUNC, 0644);
	do {
splice(STDIN_FILENO, NULL, fd, NULL, SPLICE_SIZE, 0);
		if (TST_RET < 0) {
			tst_res(TFAIL | TTERRNO, "splice failed");
			goto cleanup;
		}
	} while (TST_RET > 0);
	stat(TEST_FILENAME, &st);
	if (st.st_size != file_size) {
		tst_res(TFAIL, "file size is different from expected: %ld (%d)",
				st.st_size, file_size);
		goto cleanup;
	}
	close(fd);
	fd = open(TEST_FILENAME, O_RDONLY);
	page_size = sysconf(_SC_PAGESIZE);
	tst_res(TINFO, "checking file content");
	blocks = LTP_ALIGN(st.st_size, page_size) / page_size;
	for (block = 0; block < blocks; block++) {
		map = mmap(NULL, page_size, PROT_READ, MAP_PRIVATE,
				fd,block * page_size);
		to_check = (block+1) * page_size < (unsigned long)st.st_size ?
			page_size : st.st_size % page_size;
		for (i = 0; i < to_check; i++) {
			if (map[i] != get_letter(block * page_size + i))
				fail++;
		}
		munmap(map, page_size);
	}
	if (fail) {
		tst_res(TFAIL, "%ld unexpected bytes found", fail);
		goto cleanup;
	}
	tst_res(TPASS, "splice() system call passed");
cleanup:
	close(fd);
	exit(0);
}

void run(void)
{
	size_t i, size, written, max_pipe_size, to_write;
	char buf[BUFSIZE];
	pipe(pipe_fd);
	if (!file_size) {
		max_pipe_size = fcntl(pipe_fd[1], F_GETPIPE_SZ);
		file_size = max_pipe_size << 4;
	}
	to_write = file_size;
	if (!fork())
		do_child();
	tst_res(TINFO, "writting %d bytes", file_size);
	while (to_write > 0) {
		size = to_write > BUFSIZE ? BUFSIZE : to_write;
		for (i = 0; i < size; i++)
			buf[i] = get_letter(file_size - to_write + i);
		written = write(1, pipe_fd[1], &buf, size);
		to_write -= written;
	}
	close(pipe_fd[0]);
	close(pipe_fd[1]);
	tst_reap_children();
}

void main(){
	setup();
	cleanup();
}
