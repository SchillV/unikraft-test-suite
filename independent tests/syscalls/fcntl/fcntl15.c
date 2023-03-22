#include "incl.h"
#define DATA    "ABCDEFGHIJ"
#define DUP     0
#define OPEN    1
#define FORK_   2

char tmpname[10] = "testfile";

int fd[2];

const struct flock lock_one = {
	.l_type = F_WRLCK,
	.l_whence = 0,
	.l_start = 0L,
	.l_len = 5L,
};

const struct flock lock_two = {
	.l_type = F_WRLCK,
	.l_whence = 0,
	.l_start = 5L,
	.l_len = 5L,
};

struct tcase {
	int dup_flag;
	int test_num;
	char *dup_flag_name;
} tcases[] = {
	{DUP, 1, "dup"},
	{OPEN, 2, "open"},
	{FORK_, 3, "fork"}
};

void lock_region_two(int file_flag, int file_mode)
{
	int fd;
	fd = open(tmpname, file_flag, file_mode);
	fcntl(fd, F_SETLK, &lock_two);
	TST_CHECKPOINT_WAKE_AND_WAIT(1);
	close(fd);
}

void do_test(int file_flag, int file_mode, int dup_flag)
{
	int ret, fd;
	fd = open(tmpname, file_flag, file_mode);
	if (!fcntl(fd, F_SETLK, &lock_one))
		tst_res(TFAIL, "Succeeded to lock already locked region one");
	else
		tst_res(TPASS, "Failed to lock already locked region one");
	if (!fcntl(fd, F_SETLK, &lock_two))
		tst_res(TFAIL, "Succeeded to lock already locked region two");
	else
		tst_res(TPASS, "Failed to lock already locked region two");
	TST_CHECKPOINT_WAKE_AND_WAIT(0);
	if (fcntl(fd, F_SETLK, &lock_one))
		tst_res(TFAIL | TERRNO, "Failed to lock now unlocked region one");
	else
		tst_res(TPASS, "Succeeded to lock now ulocked region two");
	ret = fcntl(fd, F_SETLK, &lock_two);
	if (dup_flag == FORK_) {
		if (ret)
			tst_res(TPASS, "Failed to lock already locked region two");
		else
			tst_res(TFAIL, "Succeeded to lock already locked region two");
	} else {
		if (ret)
			tst_res(TFAIL, "Failed to lock now ulocked region two");
		else
			tst_res(TPASS, "Succeeded to lock now ulocked region two");
	}
	close(fd);
	TST_CHECKPOINT_WAKE(0);
}

int run_test(int file_flag, int file_mode, int dup_flag)
{
	fd[0] = open(tmpname, file_flag, file_mode);
	write(1, fd[0], DATA, 10);
	switch (dup_flag) {
	case FORK_:
		if (!fork()) {
			lock_region_two(file_flag, file_mode);
			exit(0);
		}
	break;
	case OPEN:
		fd[1] = open(tmpname, file_flag, file_mode);
	break;
	case DUP:
		fd[1] = fcntl(fd[0], F_DUPFD, 0);
	break;
	}
	fcntl(fd[0], F_SETLK, &lock_one);
	if (dup_flag != FORK_)
		fcntl(fd[1], F_SETLK, &lock_two);
	else
		TST_CHECKPOINT_WAIT(1);
	if (!fork()) {
		do_test(file_flag, file_mode, dup_flag);
		exit(0);
	}
	TST_CHECKPOINT_WAIT(0);
	tst_res(TINFO, "Closing a file descriptor in parent");
	close(fd[0]);
	TST_CHECKPOINT_WAKE_AND_WAIT(0);
	if (dup_flag != FORK_)
		close(fd[1]);
	else
		TST_CHECKPOINT_WAKE(1);
	unlink(tmpname);
	return 0;
}

int  verify_fcntl(unsigned int n)
{
	struct tcase *tc = &tcases[n];
	tst_res(TINFO, "Running test with %s", tc->dup_flag_name);
	run_test(O_CREAT | O_RDWR | O_TRUNC, 0777, tc->dup_flag);
}

void cleanup(void)
{
	size_t i;
	for (i = 0; i < ARRAY_SIZE(fd); i++) {
		if (fd[i] > 0)
			close(fd[i]);
	}
}

void main(){
	setup();
	cleanup();
}
