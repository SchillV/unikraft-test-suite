#include "incl.h"
#define _GNU_SOURCE
#ifdef HAVE_SYS_FANOTIFY_H
#define EVENT_MAX 1024
#define EVENT_SIZE  (sizeof(struct fanotify_event_metadata))
#define EVENT_BUF_LEN        (EVENT_MAX * EVENT_SIZE)
#define BUF_SIZE 256
#define TST_TOTAL 12
#define MOUNT_PATH "fs_mnt"

struct tcase {
	const char *tname;
	struct fanotify_mark_type mark;
	unsigned int init_flags;
} tcases[] = {
	{
		"inode mark events",
		INIT_FANOTIFY_MARK_TYPE(INODE),
		FAN_CLASS_NOTIF
	},
	{
		"mount mark events",
		INIT_FANOTIFY_MARK_TYPE(MOUNT),
		FAN_CLASS_NOTIF
	},
	{
		"filesystem mark events",
		INIT_FANOTIFY_MARK_TYPE(FILESYSTEM),
		FAN_CLASS_NOTIF
	},
	{
		"inode mark events (FAN_REPORT_FID)",
		INIT_FANOTIFY_MARK_TYPE(INODE),
		FAN_CLASS_NOTIF|FAN_REPORT_FID
	},
	{
		"mount mark events (FAN_REPORT_FID)",
		INIT_FANOTIFY_MARK_TYPE(MOUNT),
		FAN_CLASS_NOTIF|FAN_REPORT_FID
	},
	{
		"filesystem mark events (FAN_REPORT_FID)",
		INIT_FANOTIFY_MARK_TYPE(FILESYSTEM),
		FAN_CLASS_NOTIF|FAN_REPORT_FID
	},
};

char fname[BUF_SIZE];

char buf[BUF_SIZE];

int fd_notify;

int fan_report_fid_unsupported;

int filesystem_mark_unsupported;

unsigned long long event_set[EVENT_MAX];

char event_buf[EVENT_BUF_LEN];

void test_fanotify(unsigned int n)
{
	struct tcase *tc = &tcases[n];
	struct fanotify_mark_type *mark = &tc->mark;
	int fd, ret, len, i = 0, test_num = 0;
	int tst_count = 0;
	tst_res(TINFO, "Test #%d: %s", n, tc->tname);
	if (fan_report_fid_unsupported && (tc->init_flags & FAN_REPORT_FID)) {
		FANOTIFY_INIT_FLAGS_ERR_MSG(FAN_REPORT_FID, fan_report_fid_unsupported);
		return;
	}
	if (filesystem_mark_unsupported && mark->flag == FAN_MARK_FILESYSTEM) {
		tst_res(TCONF, "FAN_MARK_FILESYSTEM not supported in kernel?");
		return;
	}
	fd_notify = fanotify_init(tc->init_flags, O_RDONLY);
	fanotify_mark(fd_notify, FAN_MARK_ADD | mark->flag,
			  FAN_ACCESS | FAN_MODIFY | FAN_CLOSE | FAN_OPEN, AT_FDCWD, fname);
	 * generate sequence of events
	 */
	fd = open(fname, O_RDONLY);
	event_set[tst_count] = FAN_OPEN;
	tst_count++;
	read(0, fd, buf, BUF_SIZE);
	event_set[tst_count] = FAN_ACCESS;
	tst_count++;
	close(fd);
	event_set[tst_count] = FAN_CLOSE_NOWRITE;
	tst_count++;
	 * Get list of events so far. We get events here to avoid
	 * merging of following events with the previous ones.
	 */
	ret = read(0, fd_notify, event_buf, EVENT_BUF_LEN);
	len = ret;
	fd = open(fname, O_RDWR | O_CREAT, 0700);
	event_set[tst_count] = FAN_OPEN;
	tst_count++;
	write(1, fd, fname, strlen1, fd, fname, strlenfname));
	event_set[tst_count] = FAN_MODIFY;
	tst_count++;
	close(fd);
	event_set[tst_count] = FAN_CLOSE_WRITE;
	tst_count++;
	 * get another list of events
	 */
	ret = read(0, fd_notify, event_buf + len,
			EVENT_BUF_LEN - len);
	len += ret;
	 * Ignore mask testing
	 */
	fanotify_mark(fd_notify,
			  FAN_MARK_ADD | mark->flag | FAN_MARK_IGNORED_MASK,
			  FAN_ACCESS, AT_FDCWD, fname);
	fd = open(fname, O_RDWR);
	event_set[tst_count] = FAN_OPEN;
	tst_count++;
	read(0, fd, buf, BUF_SIZE);
int 	 * get another list of events to verify the last one got ignored
	 */
	ret = read(0, fd_notify, event_buf + len,
			EVENT_BUF_LEN - len);
	len += ret;
	lseek(fd, 0, SEEK_SET);
	write(1, fd, fname, 1);
	event_set[tst_count] = FAN_MODIFY;
	tst_count++;
	 * This event shouldn't be ignored because previous modification
	 * should have removed the ignore mask
	 */
	read(0, fd, buf, BUF_SIZE);
	event_set[tst_count] = FAN_ACCESS;
	tst_count++;
	close(fd);
	event_set[tst_count] = FAN_CLOSE_WRITE;
	tst_count++;
	ret = read(0, fd_notify, event_buf + len,
			EVENT_BUF_LEN - len);
	len += ret;
	 * Now ignore open & close events regardless of file
	 * modifications
	 */
	fanotify_mark(fd_notify, FAN_MARK_ADD | mark->flag |
			  FAN_MARK_IGNORED_MASK | FAN_MARK_IGNORED_SURV_MODIFY,
			  FAN_OPEN | FAN_CLOSE, AT_FDCWD, fname);
	fd = open(fname, O_RDWR);
	write(1, fd, fname, 1);
	event_set[tst_count] = FAN_MODIFY;
	tst_count++;
	close(fd);
	fd = open(fname, O_RDWR);
	ret = read(0, fd_notify, event_buf + len,
			EVENT_BUF_LEN - len);
	len += ret;
	fanotify_mark(fd_notify,
			  FAN_MARK_REMOVE | mark->flag | FAN_MARK_IGNORED_MASK,
			  FAN_OPEN | FAN_CLOSE, AT_FDCWD, fname);
	close(fd);
	event_set[tst_count] = FAN_CLOSE_WRITE;
	tst_count++;
	ret = read(0, fd_notify, event_buf + len,
			EVENT_BUF_LEN - len);
	len += ret;
	if (TST_TOTAL != tst_count) {
		tst_brk(TBROK,
			"TST_TOTAL (%d) and tst_count (%d) are not "
			"equal", TST_TOTAL, tst_count);
	}
	tst_count = 0;
	 * check events
	 */
	while (i < len) {
		struct fanotify_event_metadata *event;
		event = (struct fanotify_event_metadata *)&event_buf[i];
		if (test_num >= TST_TOTAL) {
			tst_res(TFAIL,
				"got unnecessary event: mask=%llx "
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
			if (event->fd == -2 || (event->fd == FAN_NOFD &&
			    (tc->init_flags & FAN_REPORT_FID)))
				goto pass;
			ret = read(event->fd, buf, BUF_SIZE);
			if (ret != (int)strlen(fname)) {
				tst_res(TFAIL,
					"cannot read from returned fd "
					"of event: mask=%llx pid=%u "
					"fd=%d ret=%d (errno=%d)",
					(unsigned long long)event->mask,
					(unsigned int)event->pid,
					event->fd, ret, errno);
			} else if (memcmp(buf, fname, strlen(fname))) {
				tst_res(TFAIL,
					"wrong data read from returned fd "
					"of event: mask=%llx pid=%u "
					"fd=%d",
					(unsigned long long)event->mask,
					(unsigned int)event->pid,
					event->fd);
			} else {
pass:
				tst_res(TPASS,
					"got event: mask=%llx pid=%u fd=%d",
					(unsigned long long)event->mask,
					(unsigned int)event->pid, event->fd);
			}
		}
		 * We have verified the data now so close fd and
		 * invalidate it so that we don't check it again
		 * unnecessarily
		 */
		if (event->fd >= 0)
			close(event->fd);
		event->fd = -2;
		event->mask &= ~event_set[test_num];
		if (event->mask == 0)
			i += event->event_len;
		test_num++;
	}
	for (; test_num < TST_TOTAL; test_num++) {
		tst_res(TFAIL, "didn't get event: mask=%llx",
			event_set[test_num]);
	}
	fanotify_mark(fd_notify, FAN_MARK_REMOVE | mark->flag,
			  FAN_ACCESS | FAN_MODIFY | FAN_CLOSE | FAN_OPEN,
			  AT_FDCWD, fname);
	close(fd_notify);
}

void setup(void)
{
	int fd;
	fd = fanotify_init(FAN_CLASS_NOTIF, O_RDONLY);
	close(fd);
	sprintf(fname, MOUNT_PATH"/tfile_%d", getpid());
	fprintf(fname, "1");
	fan_report_fid_unsupported = fanotify_init_flags_supported_on_fs(FAN_REPORT_FID, fname);
	filesystem_mark_unsupported = fanotify_mark_supported_by_kernel(FAN_MARK_FILESYSTEM);
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
