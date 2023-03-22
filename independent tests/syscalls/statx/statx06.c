#include "incl.h"
#define _GNU_SOURCE
#define MOUNT_POINT "mount_ext"
#define TEST_FILE MOUNT_POINT"/test_file.txt"
#define SIZE 2

int fd;

void timestamp_to_timespec(const struct statx_timestamp *timestamp,
				  struct timespec *timespec)
{
	timespec->tv_sec = timestamp->tv_sec;
	timespec->tv_nsec = timestamp->tv_nsec;
}

void clock_wait_tick(void)
{
	struct timespec res;
	unsigned int usecs;
	clock_getres(CLOCK_REALTIME_COARSE, &res);
	usecs = tst_timespec_to_us(res);
	usleep(usecs);
}

void create_file(void)
{
	if (fd > 0) {
		close(fd);
		unlink(TEST_FILE);
	}
	fd = open(TEST_FILE, O_CREAT | O_RDWR, 0666);
}

void write_file(void)
{
	char data[SIZE] = "hi";
	write(
}

void read_file(void)
{
	char data[SIZE];
	read(0, fd, data, sizeof0, fd, data, sizeofdata));
}

void change_mode(void)
{
	chmod(TEST_FILE, 0777);
}

struct test_case {
	void (*operation)(void);
	char *op_name;
} tcases[] = {
	{.operation = create_file,
	 .op_name = "Birth time"},
	{.operation = write_file,
	 .op_name = "Modified time"},
	{.operation = read_file,
	 .op_name = "Access time"},
	{.operation = change_mode,
	 .op_name = "Change time"}
};

void test_statx(unsigned int test_nr)
{
	struct statx buff;
	struct timespec before_time;
	struct timespec after_time;
	struct timespec statx_time = {0, 0};
	struct test_case *tc = &tcases[test_nr];
	clock_gettime(CLOCK_REALTIME_COARSE, &before_time);
	clock_wait_tick();
	tc->operation();
	clock_wait_tick();
	clock_gettime(CLOCK_REALTIME_COARSE, &after_time);
statx(AT_FDCWD, TEST_FILE, 0, STATX_ALL, &buff);
	if (TST_RET != 0) {
		tst_brk(TFAIL | TTERRNO,
			"statx(AT_FDCWD, %s, 0, STATX_ALL, &buff)",
			TEST_FILE);
	}
	switch (test_nr) {
	case 0:
		timestamp_to_timespec(&buff.stx_btime, &statx_time);
		break;
	case 1:
		timestamp_to_timespec(&buff.stx_mtime, &statx_time);
		break;
	case 2:
		timestamp_to_timespec(&buff.stx_atime, &statx_time);
		break;
	case 3:
		timestamp_to_timespec(&buff.stx_ctime, &statx_time);
		break;
	}
	if (tst_timespec_lt(statx_time, before_time))
		tst_res(TFAIL, "%s < before time", tc->op_name);
	else if (tst_timespec_lt(after_time, statx_time))
		tst_res(TFAIL, "%s > after_time", tc->op_name);
	else
		tst_res(TPASS, "%s Passed", tc->op_name);
}

void cleanup(void)
{
	if (fd > 0)
		close(fd);
}

void main(){
	setup();
	cleanup();
}
