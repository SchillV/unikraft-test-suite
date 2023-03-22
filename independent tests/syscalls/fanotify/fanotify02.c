#include "incl.h"
#define _GNU_SOURCE
#ifdef HAVE_SYS_FANOTIFY_H
#define EVENT_MAX 1024
#define EVENT_SIZE  (sizeof(struct fanotify_event_metadata))
#define EVENT_BUF_LEN        (EVENT_MAX * EVENT_SIZE)
#define BUF_SIZE 256
#define TST_TOTAL 8

char fname[BUF_SIZE];

char buf[BUF_SIZE];

int fd, fd_notify;

unsigned long long event_set[EVENT_MAX];

char event_buf[EVENT_BUF_LEN];

void test01(void)
{
	int ret, len, i = 0, test_num = 0;
	int tst_count = 0;
	fanotify_mark(fd_notify, FAN_MARK_ADD, FAN_ACCESS |
			  FAN_MODIFY | FAN_CLOSE | FAN_OPEN | FAN_EVENT_ON_CHILD |
			  FAN_ONDIR, AT_FDCWD, ".");
	 * generate sequence of events
	 */
	fd = open(fname, O_RDWR | O_CREAT, 0700);
	event_set[tst_count] = FAN_OPEN;
	tst_count++;
	write(1, fd, fname, strlen1, fd, fname, strlenfname));
	event_set[tst_count] = FAN_MODIFY;
	tst_count++;
	close(fd);
	event_set[tst_count] = FAN_CLOSE_WRITE;
	tst_count++;
	 * Get list of events so far. We get events here to avoid
	 * merging of following events with the previous ones.
	 */
	ret = read(0, fd_notify, event_buf,
			EVENT_BUF_LEN);
	len = ret;
	fd = open(fname, O_RDONLY);
	event_set[tst_count] = FAN_OPEN;
	tst_count++;
	read(0, fd, buf, BUF_SIZE);
	event_set[tst_count] = FAN_ACCESS;
	tst_count++;
	close(fd);
	event_set[tst_count] = FAN_CLOSE_NOWRITE;
	tst_count++;
	 * get next events
	 */
	ret = read(0, fd_notify, event_buf + len,
			EVENT_BUF_LEN - len);
	len += ret;
	 * now remove child mark
	 */
	fanotify_mark(fd_notify, FAN_MARK_REMOVE,
			  FAN_EVENT_ON_CHILD, AT_FDCWD, ".");
int 	 * Do something to verify events didn't get generated
	 */
	fd = open(fname, O_RDONLY);
	close(fd);
	fd = open(".", O_RDONLY | O_DIRECTORY);
	event_set[tst_count] = FAN_OPEN;
	tst_count++;
	close(fd);
	event_set[tst_count] = FAN_CLOSE_NOWRITE;
	tst_count++;
	 * Check events got generated only for the directory
	 */
	ret = read(0, fd_notify, event_buf + len,
			EVENT_BUF_LEN - len);
	len += ret;
	if (TST_TOTAL != tst_count) {
		tst_brk(TBROK,
			"TST_TOTAL and tst_count are not equal");
	}
	tst_count = 0;
	 * check events
	 */
	while (i < len) {
		struct fanotify_event_metadata *event;
		event = (struct fanotify_event_metadata *)&event_buf[i];
		if (test_num >= TST_TOTAL) {
			tst_res(TFAIL,
				"get unnecessary event: mask=%llx "
				"pid=%u fd=%d",
				(unsigned long long)event->mask,
				(unsigned int)event->pid, event->fd);
		} else if (!(event->mask & event_set[test_num])) {
			tst_res(TFAIL,
				"got event: mask=%llx (expected %llx) "
				"pid=%u fd=%d",
				(unsigned long long)event->mask,
				event_set[test_num],
				(unsigned int)event->pid, event->fd);
		} else if (event->pid != getpid()) {
			tst_res(TFAIL,
				"got event: mask=%llx pid=%u "
				"(expected %u) fd=%d",
				(unsigned long long)event->mask,
				(unsigned int)event->pid,
				(unsigned int)getpid(),
				event->fd);
		} else {
			tst_res(TPASS,
				"got event: mask=%llx pid=%u fd=%u",
				(unsigned long long)event->mask,
				(unsigned int)event->pid, event->fd);
		}
		event->mask &= ~event_set[test_num];
		if (event->mask == 0) {
			i += event->event_len;
			if (event->fd != FAN_NOFD)
				close(event->fd);
		}
		test_num++;
	}
	for (; test_num < TST_TOTAL; test_num++) {
		tst_res(TFAIL, "didn't get event: mask=%llx",
			event_set[test_num]);
	}
}

void setup(void)
{
	sprintf(fname, "fname_%d", getpid());
	fd_notify = fanotify_init(FAN_CLASS_NOTIF, O_RDONLY);
}

void cleanup(void)
{
	if (fd_notify > 0)
		close(fd_notify);
}

void main(){
	setup();
	cleanup();
}
