#include "incl.h"
#define DIO_MESSAGE "In dio mode"
#define NON_DIO_MESSAGE "In non dio mode"

char dev_path[1024], sys_loop_diopath[1024], backing_file_path[1024];

int dev_num, dev_fd, block_devfd, attach_flag, logical_block_size;

void check_dio_value(int flag)
{
	struct loop_info loopinfoget;
	memset(&loopinfoget, 0, sizeof(loopinfoget));
	ioctl(dev_fd, LOOP_GET_STATUS, &loopinfoget);
	tst_res(TINFO, "%s", flag ? DIO_MESSAGE : NON_DIO_MESSAGE);
	if (loopinfoget.lo_flags & LO_FLAGS_DIRECT_IO)
		tst_res(flag ? TPASS : TFAIL, "lo_flags has LO_FLAGS_DIRECT_IO flag");
	else
		tst_res(flag ? TFAIL : TPASS, "lo_flags doesn't have LO_FLAGS_DIRECT_IO flag");
	TST_ASSERT_INT(sys_loop_diopath, flag);
}

int  verify_ioctl_loop(
{
	struct loop_info loopinfo;
	memset(&loopinfo, 0, sizeof(loopinfo));
	TST_RETRY_FUNC(ioctl(dev_fd, LOOP_SET_STATUS, &loopinfo), TST_RETVAL_EQ0);
	tst_res(TINFO, "Without setting lo_offset or sizelimit");
	ioctl(dev_fd, LOOP_SET_DIRECT_IO, 1);
	check_dio_value(1);
	ioctl(dev_fd, LOOP_SET_DIRECT_IO, 0);
	check_dio_value(0);
	tst_res(TINFO, "With offset equal to logical_block_size");
	loopinfo.lo_offset = logical_block_size;
	TST_RETRY_FUNC(ioctl(dev_fd, LOOP_SET_STATUS, &loopinfo), TST_RETVAL_EQ0);
ioctl(dev_fd, LOOP_SET_DIRECT_IO, 1);
	if (TST_RET == 0) {
		tst_res(TPASS, "LOOP_SET_DIRECT_IO succeeded");
		check_dio_value(1);
		ioctl(dev_fd, LOOP_SET_DIRECT_IO, 0);
	} else {
		tst_res(TFAIL | TTERRNO, "LOOP_SET_DIRECT_IO failed");
	}
	tst_res(TINFO, "With nonzero offset less than logical_block_size");
	loopinfo.lo_offset = logical_block_size / 2;
	TST_RETRY_FUNC(ioctl(dev_fd, LOOP_SET_STATUS, &loopinfo), TST_RETVAL_EQ0);
ioctl(dev_fd, LOOP_SET_DIRECT_IO, 1);
	if (TST_RET == 0) {
		tst_res(TPASS, "LOOP_SET_DIRECT_IO succeeded, offset is ignored");
		ioctl(dev_fd, LOOP_SET_DIRECT_IO, 0);
		return;
	}
	if (TST_ERR == EINVAL)
		tst_res(TPASS | TTERRNO, "LOOP_SET_DIRECT_IO failed as expected");
	else
		tst_res(TFAIL | TTERRNO, "LOOP_SET_DIRECT_IO failed expected EINVAL got");
}

void setup(void)
{
	char bd_path[100];
	dev_num = tst_find_free_loopdev(dev_path, sizeof(dev_path));
	if (dev_num < 0)
		tst_brk(TBROK, "Failed to find free loop device");
	sprintf(sys_loop_diopath, "/sys/block/loop%d/loop/dio", dev_num);
	tst_fill_file("test.img", 0, 1024, 1024);
	tst_attach_device(dev_path, "test.img");
	attach_flag = 1;
	dev_fd = open(dev_path, O_RDWR);
	if (ioctl(dev_fd, LOOP_SET_DIRECT_IO, 0) && errno == EINVAL)
		tst_brk(TCONF, "LOOP_SET_DIRECT_IO is not supported");
	 * from __loop_update_dio():
	 *   We support direct I/O only if lo_offset is aligned with the
	 *   logical I/O size of backing device, and the logical block
	 *   size of loop is bigger than the backing device's and the loop
	 *   needn't transform transfer.
	 */
	sprintf(backing_file_path, "%s/test.img", tst_get_tmpdir());
	tst_find_backing_dev(backing_file_path, bd_path);
	block_devfd = open(bd_path, O_RDWR);
	ioctl(block_devfd, BLKSSZGET, &logical_block_size);
	tst_res(TINFO, "backing dev(%s) logical_block_size is %d", bd_path, logical_block_size);
	close(block_devfd);
	if (logical_block_size > 512)
		TST_RETRY_FUNC(ioctl(dev_fd, LOOP_SET_BLOCK_SIZE, logical_block_size), TST_RETVAL_EQ0);
}

void cleanup(void)
{
	if (dev_fd > 0)
		close(dev_fd);
	if (block_devfd > 0)
		close(block_devfd);
	if (attach_flag)
		tst_detach_device(dev_path);
}

void main(){
	setup();
	cleanup();
}
