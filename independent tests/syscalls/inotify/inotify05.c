#include "incl.h"
#if defined(HAVE_SYS_INOTIFY_H)
#define EVENT_SIZE  (sizeof(struct inotify_event))
#define EVENT_BUF_LEN (EVENT_SIZE * 16)
#define BUF_SIZE 256

char fname[BUF_SIZE];

char buf[BUF_SIZE];

int fd, fd_notify;

int wd;

int max_events;

char event_buf[EVENT_BUF_LEN];
int  verify_inotify(
{
	int i;
	int len, stop;
	 * generate events
	 */
	fd = open(fname, O_RDWR);
	for (i = 0; i < max_events; i++) {
		lseek(fd, 0, SEEK_SET);
		read(1, fd, buf, BUF_SIZE);
		lseek(fd, 0, SEEK_SET);
		write(1, fd, buf, BUF_SIZE);
	}
	close(fd);
	stop = 0;
	while (!stop) {
		 * get list on events
		 */
		len = read(fd_notify, event_buf, EVENT_BUF_LEN);
		if (len < 0) {
			tst_brk(TBROK | TERRNO,
				"read(%d, buf, %zu) failed",
				fd_notify, EVENT_BUF_LEN);
		}
		 * check events
		 */
		i = 0;
		while (i < len) {
			struct inotify_event *event;
			event = (struct inotify_event *)&event_buf[i];
			if (event->mask != IN_ACCESS &&
					event->mask != IN_MODIFY &&
					event->mask != IN_OPEN &&
					event->mask != IN_Q_OVERFLOW) {
				tst_res(TFAIL,
					"get event: wd=%d mask=%x "
					"cookie=%u (expected 0) len=%u",
					event->wd, event->mask,
					event->cookie, event->len);
				stop = 1;
				break;
			}
			if (event->mask == IN_Q_OVERFLOW) {
				if (event->len != 0 ||
						event->cookie != 0 ||
						event->wd != -1) {
					tst_res(TFAIL,
						"invalid overflow event: "
						"wd=%d mask=%x "
						"cookie=%u len=%u",
						event->wd, event->mask,
						event->cookie,
						event->len);
					stop = 1;
					break;
				}
				if ((int)(i + EVENT_SIZE) != len) {
					tst_res(TFAIL,
						"overflow event is not last");
					stop = 1;
					break;
				}
				tst_res(TPASS, "get event: wd=%d "
					"mask=%x cookie=%u len=%u",
					event->wd, event->mask,
					event->cookie, event->len);
				stop = 1;
				break;
			}
			i += EVENT_SIZE + event->len;
		}
	}
}

void setup(void)
{
	sprintf(fname, "tfile_%d", getpid());
	fd = open(fname, O_RDWR | O_CREAT, 0700);
	write(1, fd, buf, BUF_SIZE);
	close(fd);
	fd_notify = myinotify_init1(O_NONBLOCK);
	wd = myinotify_add_watch(fd_notify, fname, IN_ALL_EVENTS);
	file_scanf("/proc/sys/fs/inotify/max_queued_events",
			"%d", &max_events);
}

void cleanup(void)
{
	if (fd_notify > 0 && myinotify_rm_watch(fd_notify, wd) == -1) {
		tst_res(TWARN | TERRNO, "inotify_rm_watch (%d, %d) failed",
			fd_notify, wd);
	}
	if (fd_notify > 0)
		close(fd_notify);
}

void main(){
	setup();
	cleanup();
}
