#include "incl.h"

int file_fd, file_change_fd, file_fd_invalid;

char backing_path[1024], backing_file_path[1024], backing_file_change_path[1024];

int attach_flag, dev_fd, loop_configure_sup = 1;

char loop_ro_path[1024], dev_path[1024];

struct loop_config loopconfig;

struct tcase {
	int mode;
	int ioctl;
	char *message;
} tcases[] = {
	{O_RDONLY, LOOP_SET_FD, "Using LOOP_SET_FD to setup loopdevice"},
	{O_RDWR, LOOP_CONFIGURE, "Using LOOP_CONFIGURE with read_only flag"},
};

int  verify_ioctl_loop(unsigned int n)
{
	struct tcase *tc = &tcases[n];
	struct loop_info loopinfoget;
	if (tc->ioctl == LOOP_CONFIGURE && !loop_configure_sup) {
		tst_res(TCONF, "LOOP_CONFIGURE ioctl not supported");
		return;
	}
	tst_res(TINFO, "%s", tc->message);
	file_fd = open("test.img", tc->mode);
	if (tc->ioctl == LOOP_SET_FD) {
		ioctl(dev_fd, LOOP_SET_FD, file_fd);
	} else {
		loopconfig.fd = file_fd;
		ioctl(dev_fd, LOOP_CONFIGURE, &loopconfig);
	}
	attach_flag = 1;
	TST_ASSERT_INT(loop_ro_path, 1);
	TST_ASSERT_STR(backing_path, backing_file_path);
	memset(&loopinfoget, 0, sizeof(loopinfoget));
	ioctl(dev_fd, LOOP_GET_STATUS, &loopinfoget);
	if (loopinfoget.lo_flags & ~LO_FLAGS_READ_ONLY)
		tst_res(TFAIL, "lo_flags has unexpected %d flag", loopinfoget.lo_flags);
	else
		tst_res(TPASS, "lo_flags only has default LO_FLAGS_READ_ONLY flag");
write(dev_fd, "xx", 2);
	if (TST_RET != -1)
		tst_res(TFAIL, "write succeed unexpectedly");
	else
		tst_res(TPASS | TTERRNO, "Can not write data in RO mode");
ioctl(dev_fd, LOOP_CHANGE_FD, file_change_fd);
	if (TST_RET) {
		tst_res(TFAIL | TTERRNO, "LOOP_CHANGE_FD failed");
	} else {
		tst_res(TPASS, "LOOP_CHANGE_FD succeeded");
		TST_ASSERT_INT(loop_ro_path, 1);
		TST_ASSERT_STR(backing_path, backing_file_change_path);
	}
ioctl(dev_fd, LOOP_CHANGE_FD, file_fd_invalid);
	if (TST_RET) {
		if (TST_ERR == EINVAL)
			tst_res(TPASS | TTERRNO, "LOOP_CHANGE_FD failed as expected");
		else
			tst_res(TFAIL | TTERRNO, "LOOP_CHANGE_FD failed expected EINVAL got");
	} else {
		tst_res(TFAIL, "LOOP_CHANGE_FD succeeded");
	}
	close(file_fd);
	tst_detach_device_by_fd(dev_path, dev_fd);
	attach_flag = 0;
}

void setup(void)
{
	int dev_num;
	int ret;
	char *tmpdir = tst_get_tmpdir();
	dev_num = tst_find_free_loopdev(dev_path, sizeof(dev_path));
	if (dev_num < 0)
		tst_brk(TBROK, "Failed to find free loop device");
	tst_fill_file("test.img", 0, 1024, 10);
	tst_fill_file("test1.img", 0, 1024, 10);
	tst_fill_file("test2.img", 0, 2048, 20);
	sprintf(backing_path, "/sys/block/loop%d/loop/backing_file", dev_num);
	sprintf(backing_file_path, "%s/test.img", tmpdir);
	sprintf(backing_file_change_path, "%s/test1.img", tmpdir);
	sprintf(loop_ro_path, "/sys/block/loop%d/ro", dev_num);
	free(tmpdir);
	file_change_fd = open("test1.img", O_RDWR);
	file_fd_invalid = open("test2.img", O_RDWR);
	dev_fd = open(dev_path, O_RDWR);
	loopconfig.fd = -1;
	ret = ioctl(dev_fd, LOOP_CONFIGURE, &loopconfig);
	if (ret && errno != EBADF) {
		tst_res(TINFO | TERRNO, "LOOP_CONFIGURE is not supported");
		loop_configure_sup = 0;
	}
	loopconfig.info.lo_flags = LO_FLAGS_READ_ONLY;
}

void cleanup(void)
{
	if (dev_fd > 0)
		close(dev_fd);
	if (file_fd > 0)
		close(file_fd);
	if (file_change_fd > 0)
		close(file_change_fd);
	if (file_fd_invalid > 0)
		close(file_fd_invalid);
	if (attach_flag)
		tst_detach_device(dev_path);
}

void main(){
	setup();
	cleanup();
}
