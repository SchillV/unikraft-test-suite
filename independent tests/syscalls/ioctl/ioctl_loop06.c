#include "incl.h"

char dev_path[1024];

int dev_num, dev_fd, file_fd, attach_flag, loop_configure_sup = 1;

unsigned int invalid_value, half_value, unalign_value;

struct loop_config loopconfig;

struct tcase {
	unsigned int *setvalue;
	int ioctl_flag;
	char *message;
} tcases[] = {
	{&half_value, LOOP_SET_BLOCK_SIZE,
	"Using LOOP_SET_BLOCK_SIZE with arg < 512"},
	{&invalid_value, LOOP_SET_BLOCK_SIZE,
	"Using LOOP_SET_BLOCK_SIZE with arg > PAGE_SIZE"},
	{&unalign_value, LOOP_SET_BLOCK_SIZE,
	"Using LOOP_SET_BLOCK_SIZE with arg != power_of_2"},
	{&half_value, LOOP_CONFIGURE,
	"Using LOOP_CONFIGURE with block_size < 512"},
	{&invalid_value, LOOP_CONFIGURE,
	"Using LOOP_CONFIGURE with block_size > PAGE_SIZE"},
	{&unalign_value, LOOP_CONFIGURE,
	"Using LOOP_CONFIGURE with block_size != power_of_2"},
};

int  verify_ioctl_loop(unsigned int n)
{
	if (tcases[n].ioctl_flag == LOOP_CONFIGURE)
ioctl(dev_fd, LOOP_CONFIGURE, &loopconfig);
	else
ioctl(dev_fd, LOOP_SET_BLOCK_SIZE, *(tcases[n].setvalue));
	if (TST_RET == 0) {
		tst_res(TFAIL, "Set block size succeed unexpectedly");
		if (tcases[n].ioctl_flag == LOOP_CONFIGURE)
			tst_detach_device_by_fd(dev_path, dev_fd);
		return;
	}
	if (TST_ERR == EINVAL)
		tst_res(TPASS | TTERRNO, "Set block size failed as expected");
	else
		tst_res(TFAIL | TTERRNO, "Set block size failed expected EINVAL got");
}

void run(unsigned int n)
{
	struct tcase *tc = &tcases[n];
	tst_res(TINFO, "%s", tc->message);
	if (tc->ioctl_flag == LOOP_SET_BLOCK_SIZE) {
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
	loopconfig.block_size = *(tc->setvalue);
int 	verify_ioctl_loop(n);
}

void setup(void)
{
	unsigned int pg_size;
	int ret;
	dev_num = tst_find_free_loopdev(dev_path, sizeof(dev_path));
	if (dev_num < 0)
		tst_brk(TBROK, "Failed to find free loop device");
	tst_fill_file("test.img", 0, 1024, 1024);
	half_value = 256;
	pg_size = getpagesize();
	invalid_value = pg_size * 2 ;
	unalign_value = pg_size - 1;
	dev_fd = open(dev_path, O_RDWR);
	if (ioctl(dev_fd, LOOP_SET_BLOCK_SIZE, 512) && errno == EINVAL)
		tst_brk(TCONF, "LOOP_SET_BLOCK_SIZE is not supported");
	file_fd = open("test.img", O_RDWR);
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
