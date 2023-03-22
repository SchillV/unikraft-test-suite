#include "incl.h"
#define _GNU_SOURCE
#ifndef SYNC_FILE_RANGE_WAIT_BEFORE
#define SYNC_FILE_RANGE_WAIT_BEFORE 1
#define SYNC_FILE_RANGE_WRITE 2
#define SYNC_FILE_RANGE_WAIT_AFTER 4
#endif
#define SYNC_FILE_RANGE_INVALID 8

char filename[255];

const char spl_file[] = "/dev/null";

int fd, sfd;

int bfd = -1;
struct test_case {
	int *fd;
	off_t offset;
	off_t nbytes;
	unsigned int flags;
	int error;
} tcases[] = {
	{&bfd, 0, 1, SYNC_FILE_RANGE_WRITE, EBADF},
	{&sfd, 0, 1, SYNC_FILE_RANGE_WAIT_AFTER, ESPIPE},
	{&fd, -1, 1, SYNC_FILE_RANGE_WAIT_BEFORE, EINVAL},
	{&fd, 0, -1, SYNC_FILE_RANGE_WRITE, EINVAL},
	{&fd, 0, 1, SYNC_FILE_RANGE_INVALID, EINVAL}
};

void cleanup(void)
{
	close(fd);
}

void setup(void)
{
	if (!check_sync_file_range())
		tst_brk(TCONF, "sync_file_range() not supported");
	sprintf(filename, "tmpfile_%d", getpid());
	fd = open(filename, O_RDWR | O_CREAT, 0700);
	sfd = open(spl_file, O_RDWR | O_CREAT, 0700);
}

void run_test(unsigned int nr)
{
	struct test_case *tc = &tcases[nr];
	TST_EXP_FAIL(tst_syscall(__NR_sync_file_range, *(tc->fd),
		tc->offset, tc->nbytes, tc->flags), tc->error,
		"sync_file_range(%i, %li, %li, %i)",
		*(tc->fd), (long)tc->offset, (long)tc->nbytes, tc->flags);
}

void main(){
	setup();
	cleanup();
}
