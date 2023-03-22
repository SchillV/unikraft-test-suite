#include "incl.h"
#define _GNU_SOURCE

int fd_src;

int fd_dest;
struct timespec get_timestamp(int fd)
{
	struct stat filestat;
	fstat(fd, &filestat);
	return filestat.st_mtim;
}

int  verify_copy_file_range_timestamp(
{
	loff_t offset;
	struct timespec timestamp1, timestamp2;
	long long diff_us;
	timestamp1 = get_timestamp(fd_dest);
	usleep(1500000);
	offset = 0;
sys_copy_file_range(fd_src, &offse;
			fd_dest, 0, CONTSIZE, 0));
	if (TST_RET == -1)
		tst_brk(TBROK | TTERRNO,
				"copy_file_range unexpectedly failed");
	timestamp2 = get_timestamp(fd_dest);
	diff_us = tst_timespec_diff_us(timestamp2, timestamp1);
	if (diff_us >= 1000000 && diff_us <= 30000000)
		tst_res(TPASS, "copy_file_range sucessfully updated the timestamp");
	else
		tst_brk(TFAIL, "diff_us = %lld, copy_file_range might not update timestamp", diff_us);
}

void cleanup(void)
{
	if (fd_dest > 0)
		close(fd_dest);
	if (fd_src  > 0)
		close(fd_src);
}

void setup(void)
{
	syscall_info();
	fd_dest = open(FILE_DEST_PATH, O_RDWR | O_CREAT, 0664);
	fd_src  = open(FILE_SRC_PATH,  O_RDWR | O_CREAT, 0664);
	write(1, fd_src,  CONTENT,  CONTSIZE);
	close(fd_src);
	fd_src = open(FILE_SRC_PATH, O_RDONLY);
}

void main(){
	setup();
	cleanup();
}
