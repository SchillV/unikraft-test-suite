#include "incl.h"
#define _GNU_SOURCE
#ifdef HAVE_SYS_FANOTIFY_H
#define EVENT_MAX 1024
#define EVENT_SIZE  (sizeof(struct fanotify_event_metadata))
#define EVENT_BUF_LEN        (EVENT_MAX * EVENT_SIZE)
#define MOUNT_PATH "fs_mnt"
#define TEST_FILE MOUNT_PATH "/testfile"
#define DROP_CACHES_FILE "/proc/sys/vm/drop_caches"
#define CACHE_PRESSURE_FILE "/proc/sys/vm/vfs_cache_pressure"

int old_cache_pressure;

int fd_notify;

unsigned long long event_set[EVENT_MAX];

char event_buf[EVENT_BUF_LEN];

void fsync_file(const char *path)
{
	int fd = open(path, O_RDONLY);
	fsync(fd);
	close(fd);
}

void mount_cycle(void)
{
	umount(MOUNT_PATH);
	mount(tst_device->dev, MOUNT_PATH, tst_device->fs_type, 0, NULL);
}

int int verify_mark_removed(const char *path, const char *when)
{
	int ret;
	 * We know that inode with evictable mark was evicted when a
	 * bogus call remove ACCESS from event mask returns ENOENT.
	 */
	errno = 0;
	ret = fanotify_mark(fd_notify, FAN_MARK_REMOVE,
			    FAN_ACCESS, AT_FDCWD, path);
	if (ret == -1 && errno == ENOENT) {
		tst_res(TPASS,
			"FAN_MARK_REMOVE failed with ENOENT as expected"
			" %s", when);
		return 1;
	}
	tst_res(TFAIL | TERRNO,
		"FAN_MARK_REMOVE did not fail with ENOENT as expected"
		" %s", when);
	return 0;
}

void test_fanotify(void)
{
	int ret, len, test_num = 0;
	struct fanotify_event_metadata *event;
	int tst_count = 0;
	fd_notify = fanotify_init(FAN_CLASS_NOTIF | FAN_REPORT_FID |
				       FAN_NONBLOCK, O_RDONLY);
	 * Verify that evictable mark can be upgraded to non-evictable
	 * and cannot be downgraded to evictable.
	 */
	fanotify_mark(fd_notify, FAN_MARK_ADD | FAN_MARK_EVICTABLE,
			   FAN_ACCESS,
			   AT_FDCWD, TEST_FILE);
	fanotify_mark(fd_notify, FAN_MARK_ADD,
			   FAN_ACCESS,
			   AT_FDCWD, TEST_FILE);
	errno = 0;
	ret = fanotify_mark(fd_notify, FAN_MARK_ADD | FAN_MARK_EVICTABLE,
			    FAN_ACCESS,
			    AT_FDCWD, TEST_FILE);
	if (ret == -1 && errno == EEXIST) {
		tst_res(TPASS,
			"FAN_MARK_ADD failed with EEXIST as expected"
			" when trying to downgrade to evictable mark");
	} else {
		tst_res(TFAIL | TERRNO,
			"FAN_MARK_ADD did not fail with EEXIST as expected"
			" when trying to downgrade to evictable mark");
	}
	fanotify_mark(fd_notify, FAN_MARK_REMOVE,
			   FAN_ACCESS,
			   AT_FDCWD, TEST_FILE);
int 	verify_mark_removed(TEST_FILE, "after empty mask");
	 * Watch ATTRIB events on entire mount
	 */
	fanotify_mark(fd_notify, FAN_MARK_ADD | FAN_MARK_FILESYSTEM,
			   FAN_ATTRIB, AT_FDCWD, MOUNT_PATH);
	 * Generate events
	 */
	chmod(TEST_FILE, 0600);
	event_set[tst_count] = FAN_ATTRIB;
	tst_count++;
	ret = read(0, fd_notify, event_buf, EVENT_BUF_LEN);
	len = ret;
	 * Evictable mark on file ignores ATTRIB events
	 */
	fanotify_mark(fd_notify, FAN_MARK_ADD | FAN_MARK_EVICTABLE |
			   FAN_MARK_IGNORED_MASK | FAN_MARK_IGNORED_SURV_MODIFY,
			   FAN_ATTRIB, AT_FDCWD, TEST_FILE);
	chmod(TEST_FILE, 0600);
int 	 * Read events to verify event was ignored
	 */
	ret = read(fd_notify, event_buf + len, EVENT_BUF_LEN - len);
	if (ret < 0 && errno == EAGAIN) {
		tst_res(TPASS, "Got no events as expected");
	} else {
		tst_res(TFAIL, "Got expected events");
		len += ret;
	}
	 * drop_caches should evict inode from cache and remove evictable mark
	 */
	fsync_file(TEST_FILE);
	fprintf(DROP_CACHES_FILE, "3");
int 	verify_mark_removed(TEST_FILE, "after drop_caches");
	chmod(TEST_FILE, 0600);
	event_set[tst_count] = FAN_ATTRIB;
	tst_count++;
	ret = read(0, fd_notify, event_buf + len, EVENT_BUF_LEN - len);
	len += ret;
	 * Check events
	 */
	event = (struct fanotify_event_metadata *)event_buf;
	while (FAN_EVENT_OK(event, len) && test_num < tst_count) {
		if (!(event->mask & event_set[test_num])) {
			tst_res(TFAIL,
				"got event: mask=%llx (expected %llx)",
				(unsigned long long)event->mask,
				event_set[test_num]);
		} else {
			tst_res(TPASS,
				"got event: mask=%llx",
				(unsigned long long)event->mask);
		}
		 * Close fd and invalidate it so that we don't check it again
		 * unnecessarily
		 */
		if (event->fd >= 0)
			close(event->fd);
		event->fd = FAN_NOFD;
		event->mask &= ~event_set[test_num];
		if (event->mask == 0)
			event = FAN_EVENT_NEXT(event, len);
		test_num++;
	}
	while (FAN_EVENT_OK(event, len)) {
		tst_res(TFAIL,
			"got unnecessary event: mask=%llx",
			(unsigned long long)event->mask);
		if (event->fd != FAN_NOFD)
			close(event->fd);
		event = FAN_EVENT_NEXT(event, len);
	}
	close(fd_notify);
	mount_cycle();
}

void setup(void)
{
	touch(TEST_FILE, 0666, NULL);
	REQUIRE_MARK_TYPE_SUPPORTED_BY_KERNEL(FAN_MARK_EVICTABLE);
	REQUIRE_FANOTIFY_EVENTS_SUPPORTED_ON_FS(FAN_CLASS_NOTIF|FAN_REPORT_FID,
						FAN_MARK_FILESYSTEM,
						FAN_ATTRIB, ".");
	file_scanf(CACHE_PRESSURE_FILE, "%d", &old_cache_pressure);
	fprintf(CACHE_PRESSURE_FILE, "500");
}

void cleanup(void)
{
	if (fd_notify > 0)
		close(fd_notify);
	fprintf(CACHE_PRESSURE_FILE, "%d", old_cache_pressure);
}

void main(){
	setup();
	cleanup();
}
