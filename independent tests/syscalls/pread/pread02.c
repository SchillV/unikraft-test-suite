#include "incl.h"
#define PREAD_TEMPFILE  "pread_file"
#define PREAD_TEMPDIR	"pread_dir"
#define K1              1024

int pipe_fd[2], fd, dir_fd;
struct test_case_t {
	int *fd;
	size_t nb;
	off_t offst;
	char *desc;
	int exp_errno;
} tcases[] = {
	{&pipe_fd[0], K1, 0, "file descriptor is a PIPE or FIFO", ESPIPE},
	{&fd, K1, -1, "specified offset is negative", EINVAL},
	{&dir_fd, K1, 0, "file descriptor is a directory", EISDIR}
};

int  verify_pread(unsigned int n)
{
	struct test_case_t *tc = &tcases[n];
	char buf[K1];
	TST_EXP_FAIL2(pread(*tc->fd, &buf, tc->nb, tc->offst), tc->exp_errno,
		"pread(%d, %zu, %lld) %s", *tc->fd, tc->nb, (long long)tc->offst, tc->desc);
}

void setup(void)
{
	pipe(pipe_fd);
	write(1, pipe_fd[1], "x", 1);
	fd = open(PREAD_TEMPFILE, O_RDWR | O_CREAT, 0666);
	mkdir(PREAD_TEMPDIR, 0777);
	dir_fd = open(PREAD_TEMPDIR, O_RDONLY);
}

void cleanup(void)
{
	int i;
	for (i = 0; i < 2; i++) {
		if (pipe_fd[i] > 0)
			close(pipe_fd[i]);
	}
	if (fd > 0)
		close(fd);
	if (dir_fd > 0)
		close(dir_fd);
}

void main(){
	setup();
	cleanup();
}
