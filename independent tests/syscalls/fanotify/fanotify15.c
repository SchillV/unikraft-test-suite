#include "incl.h"
#define _GNU_SOURCE
#ifdef HAVE_SYS_FANOTIFY_H
#define EVENT_MAX 10
#define EVENT_SIZE (sizeof(struct fanotify_event_metadata) + \
		    sizeof(struct fanotify_event_info_fid))
#define EVENT_BUF_LEN (EVENT_MAX * EVENT_SIZE * 2)
#define MOUNT_POINT "mntpoint"
#define TEST_DIR MOUNT_POINT"/test_dir"
#define DIR1 TEST_DIR"/dir1"
#define DIR2 TEST_DIR"/dir2"
#define FILE1 TEST_DIR"/file1"
#define FILE2 TEST_DIR"/file2"
#if defined(HAVE_NAME_TO_HANDLE_AT)
struct event_t {
	unsigned long long mask;
	struct fanotify_fid_t *fid;
};

int fanotify_fd;

char events_buf[EVENT_BUF_LEN];

struct event_t event_set[EVENT_MAX];

struct test_case_t {
	const char *tname;
	struct fanotify_mark_type mark;
	unsigned long mask;
} test_cases[] = {
	{
		"FAN_REPORT_FID on filesystem including FAN_DELETE_SELF",
		INIT_FANOTIFY_MARK_TYPE(FILESYSTEM),
		FAN_DELETE_SELF,
	},
	{
		"FAN_REPORT_FID on directory with FAN_EVENT_ON_CHILD",
		INIT_FANOTIFY_MARK_TYPE(INODE),
		FAN_EVENT_ON_CHILD,
	},
};

void do_test(unsigned int number)
{
	int i, fd, len, count = 0;
	struct file_handle *event_file_handle;
	struct fanotify_event_metadata *metadata;
	struct fanotify_event_info_fid *event_fid;
	struct test_case_t *tc = &test_cases[number];
	struct fanotify_mark_type *mark = &tc->mark;
	struct fanotify_fid_t root_fid, dir_fid, file_fid;
	tst_res(TINFO, "Test #%d: %s", number, tc->tname);
	fanotify_mark(fanotify_fd, FAN_MARK_ADD | mark->flag, tc->mask |
				FAN_CREATE | FAN_DELETE | FAN_MOVE |
				FAN_MODIFY | FAN_ONDIR,
				AT_FDCWD, TEST_DIR);
	fanotify_save_fid(TEST_DIR, &root_fid);
	event_set[count].mask = FAN_CREATE | FAN_MOVE | FAN_DELETE;
	event_set[count].fid = &root_fid;
	count++;
	fd = creat(FILE1, 0644);
	close(fd);
	fanotify_save_fid(FILE1, &file_fid);
	if (mark->flag == FAN_MARK_INODE &&
	    fanotify_mark(fanotify_fd, FAN_MARK_ADD | mark->flag,
			  FAN_MODIFY | FAN_DELETE_SELF,
			  AT_FDCWD, FILE1) == -1) {
		tst_brk(TBROK | TERRNO,
			"fanotify_mark(%d, FAN_MARK_ADD | %s, "
			"FAN_DELETE_SELF, AT_FDCWD, %s) failed",
			fanotify_fd, mark->name, FILE1);
	}
	 * Event on child file is not merged with dirent events.
	 * FAN_MODIFY event reported on file mark should be merged with the
	 * FAN_MODIFY event reported on parent directory watch.
	 */
	event_set[count].mask = FAN_MODIFY;
	event_set[count].fid = &file_fid;
	count++;
	truncate(FILE1, 1);
	rename(FILE1, FILE2);
	 * FAN_DELETE_SELF may be merged with FAN_MODIFY event above.
	 */
	event_set[count].mask = FAN_DELETE_SELF;
	event_set[count].fid = &file_fid;
	count++;
	unlink(FILE2);
	len = read(0, fanotify_fd, events_buf, EVENT_BUF_LEN);
	 * Generate a sequence of events on a directory. Subsequent events
	 * are merged, so it's required that we set FAN_ONDIR once in
	 * order to acknowledge that changes related to a subdirectory
	 * took place. Events on subdirectories are not merged with events
	 * on non-subdirectories.
	 */
	event_set[count].mask = FAN_ONDIR | FAN_CREATE | FAN_MOVE | FAN_DELETE;
	event_set[count].fid = &root_fid;
	count++;
	mkdir(DIR1, 0755);
	fanotify_save_fid(DIR1, &dir_fid);
	if (mark->flag == FAN_MARK_INODE &&
	    fanotify_mark(fanotify_fd, FAN_MARK_ADD | mark->flag,
			  FAN_DELETE_SELF | FAN_ONDIR,
			  AT_FDCWD, DIR1) == -1) {
		tst_brk(TBROK | TERRNO,
			"fanotify_mark(%d, FAN_MARK_ADD | %s, "
			"FAN_DELETE_SELF | FAN_ONDIR, AT_FDCWD, %s) failed",
			fanotify_fd, mark->name, DIR1);
	}
	rename(DIR1, DIR2);
	event_set[count].mask = FAN_ONDIR | FAN_DELETE_SELF;
	event_set[count].fid = &dir_fid;
	count++;
	rmdir(DIR2);
	len += read(0, fanotify_fd, events_buf + len, EVENT_BUF_LEN - len);
	 * Cleanup the mark
	 */
	fanotify_mark(fanotify_fd, FAN_MARK_FLUSH | mark->flag, 0,
			  AT_FDCWD, TEST_DIR);
	for (i = 0, metadata = (struct fanotify_event_metadata *) events_buf;
		FAN_EVENT_OK(metadata, len); i++) {
		struct event_t *expected = &event_set[i];
		event_fid = (struct fanotify_event_info_fid *) (metadata + 1);
		event_file_handle = (struct file_handle *) event_fid->handle;
		if (i >= count) {
			tst_res(TFAIL,
				"got unnecessary event: mask=%llx "
				"pid=%u fd=%d",
				(unsigned long long) metadata->mask,
				metadata->pid,
				metadata->fd);
			metadata->mask = 0;
		} else if (metadata->fd != FAN_NOFD) {
			tst_res(TFAIL,
				"Received unexpected file descriptor %d in "
				"event. Expected to get FAN_NOFD(%d)",
				metadata->fd, FAN_NOFD);
		} else if (!(metadata->mask & expected->mask)) {
			tst_res(TFAIL,
				"Got event: mask=%llx (expected %llx) "
				"pid=%u fd=%d",
				(unsigned long long) metadata->mask,
				expected->mask, (unsigned int) metadata->pid,
				metadata->fd);
		} else if (metadata->pid != getpid()) {
			tst_res(TFAIL,
				"Got event: mask=%llx pid=%u "
				"(expected %u) fd=%d",
				(unsigned long long) metadata->mask,
				(unsigned int) metadata->pid,
				(unsigned int) getpid(),
				metadata->fd);
		} else if (event_file_handle->handle_bytes !=
			   expected->fid->handle.handle_bytes) {
			tst_res(TFAIL,
				"Got event: handle_bytes (%x) returned in "
				"event does not equal handle_bytes (%x) "
				"retunred in name_to_handle_at(2)",
				event_file_handle->handle_bytes,
				expected->fid->handle.handle_bytes);
		} else if (event_file_handle->handle_type !=
			   expected->fid->handle.handle_type) {
			tst_res(TFAIL,
				"handle_type (%x) returned in event does not "
				"equal to handle_type (%x) returned in "
				"name_to_handle_at(2)",
				event_file_handle->handle_type,
				expected->fid->handle.handle_type);
		} else if (memcmp(event_file_handle->f_handle,
				  expected->fid->handle.f_handle,
				  expected->fid->handle.handle_bytes) != 0) {
			tst_res(TFAIL,
				"file_handle returned in event does not match "
				"the file_handle returned in "
				"name_to_handle_at(2)");
		} else if (memcmp(&event_fid->fsid, &expected->fid->fsid,
				  sizeof(event_fid->fsid)) != 0) {
			tst_res(TFAIL,
				"event_fid->fsid != stats.f_fsid that was "
				"obtained via statfs(2)");
		} else {
			tst_res(TPASS,
				"Got event: mask=%llx, pid=%u, "
				"fid=%x.%x.%lx values",
				metadata->mask,
				getpid(),
				FSID_VAL_MEMBER(event_fid->fsid, 0),
				FSID_VAL_MEMBER(event_fid->fsid, 1),
				*(unsigned long *)
				event_file_handle->f_handle);
		}
		metadata->mask  &= ~expected->mask;
		if (metadata->mask == 0)
			metadata = FAN_EVENT_NEXT(metadata, len);
	}
	for (; i < count; i++)
		tst_res(TFAIL,
			"Didn't receive event: mask=%llx",
			event_set[i].mask);
}

void do_setup(void)
{
	mkdir(TEST_DIR, 0755);
	REQUIRE_FANOTIFY_INIT_FLAGS_SUPPORTED_ON_FS(FAN_REPORT_FID, TEST_DIR);
	fanotify_fd = fanotify_init(FAN_REPORT_FID, O_RDONLY);
}

void do_cleanup(void)
{
	if (fanotify_fd > 0)
		close(fanotify_fd);
}

void main(){
	setup();
	cleanup();
}
