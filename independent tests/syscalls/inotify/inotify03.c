#include "incl.h"
#if defined(HAVE_SYS_INOTIFY_H)
#define EVENT_MAX 1024
#define EVENT_SIZE (sizeof(struct inotify_event))
#define EVENT_BUF_LEN		(EVENT_MAX * (EVENT_SIZE + 16))
#define BUF_SIZE 1024

char fname[BUF_SIZE];

int fd, fd_notify;

int wd;

unsigned int event_set[EVENT_MAX];

char event_buf[EVENT_BUF_LEN];
#define DIR_MODE	(S_IRWXU | S_IRUSR | S_IXUSR | S_IRGRP | S_IXGRP)

char *mntpoint = "mntpoint";

int mount_flag;
int  verify_inotify(
{
	int ret;
	int len, i, test_num;
	int test_cnt = 0;
	mount(tst_device->dev, mntpoint, tst_device->fs_type, 0, NULL);
	mount_flag = 1;
	wd = myinotify_add_watch(fd_notify, fname, IN_ALL_EVENTS);
	event_set[test_cnt] = IN_UNMOUNT;
	test_cnt++;
	event_set[test_cnt] = IN_IGNORED;
	test_cnt++;
	test_cnt++;
	tst_res(TINFO, "umount %s", tst_device->dev);
tst_umount(mntpoint);
	if (TST_RET != 0) {
		tst_brk(TBROK, "umount(2) Failed "
			"while unmounting errno = %d : %s",
			TST_ERR, strerror(TST_ERR));
	}
	mount_flag = 0;
	len = read(fd_notify, event_buf, EVENT_BUF_LEN);
	if (len < 0) {
		tst_brk(TBROK | TERRNO,
			"read(%d, buf, %zu) failed", fd_notify, EVENT_BUF_LEN);
	}
	test_num = 0;
	i = 0;
	while (i < len) {
		struct inotify_event *event;
		event = (struct inotify_event *)&event_buf[i];
		if (test_num >= (test_cnt - 1)) {
			tst_res(TFAIL,
				"get unnecessary event: wd=%d mask=%x "
				"cookie=%u len=%u",
				event->wd, event->mask,
				event->cookie, event->len);
		} else if (event_set[test_num] == event->mask) {
			tst_res(TPASS, "get event: wd=%d mask=%x"
				" cookie=%u len=%u",
				event->wd, event->mask,
				event->cookie, event->len);
		} else {
			tst_res(TFAIL, "get event: wd=%d mask=%x "
				"(expected %x) cookie=%u len=%u",
				event->wd, event->mask,
				event_set[test_num],
				event->cookie, event->len);
		}
		test_num++;
		i += EVENT_SIZE + event->len;
	}
	for (; test_num < test_cnt - 1; test_num++) {
		tst_res(TFAIL, "don't get event: mask=%x ",
			event_set[test_num]);
	}
	ret = myinotify_rm_watch(fd_notify, wd);
	if (ret != -1 || errno != EINVAL)
		tst_res(TFAIL | TERRNO,
			"inotify_rm_watch (%d, %d) didn't return EINVAL",
			fd_notify, wd);
	else
		tst_res(TPASS, "inotify_rm_watch (%d, %d) returned EINVAL",
			fd_notify, wd);
}

void setup(void)
{
	int ret;
	mkdir(mntpoint, DIR_MODE);
	mount(tst_device->dev, mntpoint, tst_device->fs_type, 0, NULL);
	mount_flag = 1;
	sprintf(fname, "%s/tfile_%d", mntpoint, getpid());
	fd = open(fname, O_RDWR | O_CREAT, 0700);
	ret = write(fd, fname, 1);
	if (ret == -1) {
		tst_brk(TBROK | TERRNO,
			 "write(%d, %s, 1) failed", fd, fname);
	}
	close(fd);
	fd_notify = myinotify_init();
	tst_umount(mntpoint);
	mount_flag = 0;
}

void cleanup(void)
{
	if (fd_notify > 0)
		close(fd_notify);
	if (mount_flag) {
tst_umount(mntpoint);
		if (TST_RET != 0)
			tst_res(TWARN | TTERRNO, "umount(%s) failed",
				mntpoint);
	}
}

void main(){
	setup();
	cleanup();
}
