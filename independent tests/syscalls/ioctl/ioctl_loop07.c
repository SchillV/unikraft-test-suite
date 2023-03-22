#include "incl.h"

char dev_path[1024], sys_loop_sizepath[1024], sys_loop_sizelimitpath[1024];

int dev_num, dev_fd, file_fd, attach_flag, loop_configure_sup = 1;

struct loop_config loopconfig;

struct tcase {
	unsigned int set_sizelimit;
	unsigned int exp_loopsize;
	int ioctl_flag;
	char *message;
} tcases[] = {
	{1024 * 4096, 2048, LOOP_SET_STATUS64,
	"When sizelimit is greater than loopsize by using LOOP_SET_STATUS64"},
	{1024 * 512, 1024, LOOP_SET_STATUS64,
	"When sizelimit is less than loopsize by using LOOP_SET_STATUS64"},
	{1024 * 4096, 2048, LOOP_CONFIGURE,
	"When sizelimit is greater than loopsize by using LOOP_CONFIGURE"},
	{1024 * 512, 1024, LOOP_CONFIGURE,
	"When sizelimit is less than loopsize by using LOOP_CONFIGURE"},
};

int  verify_ioctl_loop(unsigned int n)
{
	struct tcase *tc = &tcases[n];
	struct loop_info64 loopinfo, loopinfoget;
	memset(&loopinfo, 0, sizeof(loopinfo));
	memset(&loopinfoget, 0, sizeof(loopinfoget));
	if (tc->ioctl_flag == LOOP_CONFIGURE) {
		ioctl(dev_fd, LOOP_CONFIGURE, &loopconfig);
	} else {
		loopinfo.lo_sizelimit = tc->set_sizelimit;
		TST_RETRY_FUNC(ioctl(dev_fd, LOOP_SET_STATUS64, &loopinfo), TST_RETVAL_EQ0);
	}
	TST_ASSERT_INT(sys_loop_sizepath, tc->exp_loopsize);
	TST_ASSERT_INT(sys_loop_sizelimitpath, tc->set_sizelimit);
	ioctl(dev_fd, LOOP_GET_STATUS64, &loopinfoget);
	if (loopinfoget.lo_sizelimit == tc->set_sizelimit)
		tst_res(TPASS, "LOOP_GET_STATUS64 gets correct lo_sizelimit(%d)", tc->set_sizelimit);
	else
		tst_res(TFAIL, "LOOP_GET_STATUS64 gets wrong lo_sizelimit(%llu), expect %d",
				loopinfoget.lo_sizelimit, tc->set_sizelimit);
	if (tc->ioctl_flag == LOOP_CONFIGURE) {
		tst_detach_device_by_fd(dev_path, dev_fd);
	} else {
		loopinfo.lo_sizelimit = 0;
		TST_RETRY_FUNC(ioctl(dev_fd, LOOP_SET_STATUS, &loopinfo), TST_RETVAL_EQ0);
	}
}

void run(unsigned int n)
{
	struct tcase *tc = &tcases[n];
	tst_res(TINFO, "%s", tc->message);
	if (tc->ioctl_flag == LOOP_SET_STATUS64) {
		if (!attach_flag) {
			tst_attach_device(dev_path, "test.img");
			attach_flag = 1;
		}
int 		verify_ioctl_loop(n);
		return;
	}
	if (tc->ioctl_flag == LOOP_CONFIGURE && !loop_configure_sup) {
		tst_res(TCONF, "LOOP_CONFIGURE ioctl not supported");
		return;
	}
	if (attach_flag) {
		tst_detach_device_by_fd(dev_path, dev_fd);
		attach_flag = 0;
	}
	loopconfig.info.lo_sizelimit = tc->set_sizelimit;
int 	verify_ioctl_loop(n);
}

void setup(void)
{
	int ret;
	dev_num = tst_find_free_loopdev(dev_path, sizeof(dev_path));
	if (dev_num < 0)
		tst_brk(TBROK, "Failed to find free loop device");
	tst_fill_file("test.img", 0, 1024 * 1024, 1);
	tst_attach_device(dev_path, "test.img");
	attach_flag = 1;
	sprintf(sys_loop_sizepath, "/sys/block/loop%d/size", dev_num);
	sprintf(sys_loop_sizelimitpath, "/sys/block/loop%d/loop/sizelimit", dev_num);
	tst_detach_device(dev_path);
	attach_flag = 0;
	tst_res(TINFO, "original loop size 2048 sectors");
	file_fd = open("test.img", O_RDWR);
	dev_fd = open(dev_path, O_RDWR);
	loopconfig.fd = -1;
	ret = ioctl(dev_fd, LOOP_CONFIGURE, &loopconfig);
	if (ret && errno != EBADF) {
		tst_res(TINFO | TERRNO, "LOOP_CONFIGURE is not supported");
		loop_configure_sup = 0;
		return;
	}
	loopconfig.fd = file_fd;
}

void cleanup(void)
{
	if (dev_fd > 0)
		close(dev_fd);
	if (file_fd > 0)
		close(file_fd);
	if (attach_flag)
		tst_detach_device(dev_path);
}

void main(){
	setup();
	cleanup();
}
