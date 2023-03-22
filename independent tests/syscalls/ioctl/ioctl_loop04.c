#include "incl.h"
#define OLD_SIZE 10240
#define NEW_SIZE 5120

char dev_path[1024], sys_loop_sizepath[1024];

char *wrbuf;

int dev_num, dev_fd, file_fd, attach_flag;

int  verify_ioctl_loop(
{
	struct loop_info loopinfoget;
	memset(&loopinfoget, 0, sizeof(loopinfoget));
	tst_fill_file("test.img", 0, 1024, OLD_SIZE/1024);
	tst_attach_device(dev_path, "test.img");
	attach_flag = 1;
	TST_ASSERT_INT(sys_loop_sizepath, OLD_SIZE/512);
	file_fd = open("test.img", O_RDWR);
	ioctl(dev_fd, LOOP_GET_STATUS, &loopinfoget);
	if (loopinfoget.lo_flags & LO_FLAGS_READ_ONLY)
		tst_brk(TCONF, "Current environment has unexpected LO_FLAGS_READ_ONLY flag");
	truncate("test.img", NEW_SIZE);
	ioctl(dev_fd, LOOP_SET_CAPACITY);
	lseek(dev_fd, 0, SEEK_SET);
write(dev_fd, wrbuf, OLD_SIZE);
	if (TST_RET == NEW_SIZE) {
		tst_res(TPASS, "LOOP_SET_CAPACITY set loop size to %d", NEW_SIZE);
	} else {
		tst_res(TFAIL, "LOOP_SET_CAPACITY didn't set loop size to %d, its size is %ld",
				NEW_SIZE, TST_RET);
	}
	TST_ASSERT_INT(sys_loop_sizepath, NEW_SIZE/512);
	close(file_fd);
	tst_detach_device_by_fd(dev_path, dev_fd);
	unlink("test.img");
	attach_flag = 0;
}

void setup(void)
{
	dev_num = tst_find_free_loopdev(dev_path, sizeof(dev_path));
	if (dev_num < 0)
		tst_brk(TBROK, "Failed to find free loop device");
	wrbuf = malloc(OLD_SIZE);
	memset(wrbuf, 'x', OLD_SIZE);
	sprintf(sys_loop_sizepath, "/sys/block/loop%d/size", dev_num);
	dev_fd = open(dev_path, O_RDWR);
}

void cleanup(void)
{
	if (dev_fd > 0)
		close(dev_fd);
	if (file_fd > 0)
		close(file_fd);
	if (wrbuf)
		free(wrbuf);
	if (attach_flag)
		tst_detach_device(dev_path);
}

void main(){
	setup();
	cleanup();
}
