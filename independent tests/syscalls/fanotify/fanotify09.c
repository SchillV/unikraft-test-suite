#include "incl.h"
#define _GNU_SOURCE
#ifdef HAVE_SYS_FANOTIFY_H
#define EVENT_MAX 1024
#define EVENT_SIZE  (sizeof(struct fanotify_event_metadata))
#define EVENT_BUF_LEN        (EVENT_MAX * EVENT_SIZE)
#define NUM_GROUPS 3
#define BUF_SIZE 256

char fname[BUF_SIZE];

char symlnk[BUF_SIZE];

char fdpath[BUF_SIZE];

int fd_notify[NUM_GROUPS];

char event_buf[EVENT_BUF_LEN];
#define MOUNT_PATH "fs_mnt"
#define MOUNT_NAME "mntpoint"
#define DIR_NAME "testdir"
#define FILE2_NAME "testfile"

int mount_created;

int fan_report_dfid_unsupported;

int ignore_mark_unsupported;

struct tcase {
	const char *tname;
	struct fanotify_mark_type mark;
	unsigned int ondir;
	unsigned int ignore;
	unsigned int ignore_flags;
	unsigned int report_name;
	const char *event_path;
	int nevents;
	unsigned int nonfirst_event;
} tcases[] = {
	{
		.tname = "Events on non-dir child with both parent and mount marks",
		.mark = INIT_FANOTIFY_MARK_TYPE(MOUNT),
		.event_path = DIR_NAME,
		.nevents = 1,
	},
	{
		.tname = "Events on non-dir child and subdir with both parent and mount marks",
		.mark = INIT_FANOTIFY_MARK_TYPE(MOUNT),
		.ondir = FAN_ONDIR,
		.event_path = DIR_NAME,
		.nevents = 2,
	},
	{
		.tname = "Events on non-dir child and parent with both parent and mount marks",
		.mark = INIT_FANOTIFY_MARK_TYPE(MOUNT),
		.ondir = FAN_ONDIR,
		.event_path = ".",
		.nevents = 2,
	},
	{
		.tname = "Events on non-dir child and subdir with both parent and subdir marks",
		.mark = INIT_FANOTIFY_MARK_TYPE(INODE),
		.ondir = FAN_ONDIR,
		.event_path = DIR_NAME,
		.nevents = 2,
	},
	{
		.tname = "Events on non-dir children with both parent and mount marks",
		.mark = INIT_FANOTIFY_MARK_TYPE(MOUNT),
		.event_path = FILE2_NAME,
		.nevents = 2,
		.nonfirst_event = FAN_CLOSE_NOWRITE,
	},
	{
		.tname = "Events on non-dir child with both parent and mount marks and filename info",
		.mark = INIT_FANOTIFY_MARK_TYPE(MOUNT),
		.report_name = FAN_REPORT_DFID_NAME,
		.event_path = FILE2_NAME,
		.nevents = 2,
		.nonfirst_event = FAN_CLOSE_NOWRITE,
	},
	{
		.tname = "Events on non-dir child with ignore mask on parent",
		.mark = INIT_FANOTIFY_MARK_TYPE(MOUNT),
		.ignore = FAN_MARK_IGNORED_MASK,
		.event_path = DIR_NAME,
		.nevents = 1,
	},
	{
		.tname = "Events on non-dir children with surviving ignore mask on parent",
		.mark = INIT_FANOTIFY_MARK_TYPE(MOUNT),
		.ignore = FAN_MARK_IGNORED_MASK | FAN_MARK_IGNORED_SURV_MODIFY,
		.event_path = FILE2_NAME,
		.nevents = 2,
		.nonfirst_event = FAN_CLOSE_NOWRITE,
	},
	{
		.tname = "Events on dir with ignore mask that does not apply to dirs",
		.mark = INIT_FANOTIFY_MARK_TYPE(MOUNT),
		.ondir = FAN_ONDIR,
		.ignore = FAN_MARK_IGNORE_SURV,
		.event_path = ".",
		.nevents = 2,
		.nonfirst_event = FAN_CLOSE_NOWRITE,
	},
	{
		.tname = "Events on dir with ignore mask that does apply to dirs",
		.mark = INIT_FANOTIFY_MARK_TYPE(MOUNT),
		.ondir = FAN_ONDIR,
		.ignore = FAN_MARK_IGNORE_SURV,
		.ignore_flags = FAN_ONDIR,
		.event_path = ".",
		.nevents = 2,
	},
	{
		.tname = "Events on child with ignore mask on parent that does not apply to children",
		.mark = INIT_FANOTIFY_MARK_TYPE(MOUNT),
		.ignore = FAN_MARK_IGNORE_SURV,
		.event_path = FILE2_NAME,
		.nevents = 2,
		.nonfirst_event = FAN_CLOSE_NOWRITE,
	},
	{
		.tname = "Events on child with ignore mask on parent that does apply to children",
		.mark = INIT_FANOTIFY_MARK_TYPE(MOUNT),
		.ignore = FAN_MARK_IGNORE_SURV,
		.ignore_flags = FAN_EVENT_ON_CHILD,
		.event_path = FILE2_NAME,
		.nevents = 2,
	},
	{
		.tname = "Events on subdir with ignore mask on parent that does not apply to children",
		.mark = INIT_FANOTIFY_MARK_TYPE(MOUNT),
		.ondir = FAN_ONDIR,
		.ignore = FAN_MARK_IGNORE_SURV,
		.ignore_flags = FAN_ONDIR,
		.event_path = DIR_NAME,
		.nevents = 2,
		.nonfirst_event = FAN_CLOSE_NOWRITE,
	},
	{
		.tname = "Events on subdir with ignore mask on parent that does not apply to dirs",
		.mark = INIT_FANOTIFY_MARK_TYPE(MOUNT),
		.ondir = FAN_ONDIR,
		.ignore = FAN_MARK_IGNORE_SURV,
		.ignore_flags = FAN_EVENT_ON_CHILD,
		.event_path = DIR_NAME,
		.nevents = 2,
		.nonfirst_event = FAN_CLOSE_NOWRITE,
	},
	{
		.tname = "Events on subdir with ignore mask on parent that does apply to subdirs",
		.mark = INIT_FANOTIFY_MARK_TYPE(MOUNT),
		.ondir = FAN_ONDIR,
		.ignore = FAN_MARK_IGNORE_SURV,
		.ignore_flags = FAN_EVENT_ON_CHILD | FAN_ONDIR,
		.event_path = DIR_NAME,
		.nevents = 2,
	},
};

void create_fanotify_groups(struct tcase *tc)
{
	struct fanotify_mark_type *mark = &tc->mark;
	int i;
	for (i = 0; i < NUM_GROUPS; i++) {
		 * The first group may request events with filename info and
		 * events on subdirs and always request events on children.
		 */
		unsigned int report_name = tc->report_name;
		unsigned int mask_flags = tc->ondir | FAN_EVENT_ON_CHILD;
		unsigned int parent_mask, ignore_mask, ignore = 0;
		 * The non-first groups may request events on children and
		 * subdirs only when setting an ignore mask on parent dir.
		 * The parent ignore mask may request to ignore events on
		 * children or subdirs.
		 */
		if (i > 0) {
			ignore = tc->ignore;
			report_name = 0;
			if (!ignore)
				mask_flags = 0;
		}
		fd_notify[i] = fanotify_init(FAN_CLASS_NOTIF | report_name |
						  FAN_NONBLOCK, O_RDONLY);
		 * Add subdir or mount mark for each group with CLOSE event,
		 * but only the first group requests events on dir.
		 */
		fanotify_mark(fd_notify[i],
				    FAN_MARK_ADD | mark->flag,
				    FAN_CLOSE_NOWRITE | mask_flags,
				    AT_FDCWD, tc->event_path);
		 * Add inode mark on parent for each group with MODIFY event,
		 * but only the first group requests events on child.
		 * The one mark with FAN_EVENT_ON_CHILD is needed for
		 * setting the DCACHE_FSNOTIFY_PARENT_WATCHED dentry flag.
		 *
		 * The inode mark on non-first group is either with FAN_MODIFY
		 * in mask or FAN_CLOSE_NOWRITE in ignore mask. In either case,
		 * it is not expected to get the modify event on a child, nor
		 * the close event on dir.
		 */
		parent_mask = FAN_MODIFY | tc->ondir | mask_flags;
		ignore_mask = FAN_CLOSE_NOWRITE | tc->ignore_flags;
		fanotify_mark(fd_notify[i], FAN_MARK_ADD | ignore,
				    ignore ? ignore_mask : parent_mask,
				    AT_FDCWD, ".");
	}
}

void cleanup_fanotify_groups(void)
{
	unsigned int i;
	for (i = 0; i < NUM_GROUPS; i++) {
		if (fd_notify[i] > 0)
			close(fd_notify[i]);
	}
}

void check_ignore_mask(int fd)
{
	unsigned int ignored_mask, mflags;
	char procfdinfo[100];
	sprintf(procfdinfo, "/proc/%d/fdinfo/%d", (int)getpid(), fd);
	if (FILE_LINES_SCANF(procfdinfo, "fanotify ino:%*x sdev:%*x mflags: %x mask:0 ignored_mask:%x",
				&mflags, &ignored_mask) || !ignored_mask) {
		tst_res(TFAIL, "The ignore mask did not survive");
	} else {
		tst_res(TPASS, "Found mark with ignore mask (ignored_mask=%x, mflags=%x) in %s",
				ignored_mask, mflags, procfdinfo);
	}
}

void event_res(int ttype, int group,
		      struct fanotify_event_metadata *event,
		      const char *filename)
{
	if (event->fd != FAN_NOFD) {
		int len = 0;
		sprintf(symlnk, "/proc/self/fd/%d", event->fd);
		len = readlink(symlnk, fdpath, sizeof(fdpath));
		if (len < 0)
			len = 0;
		fdpath[len] = 0;
		filename = fdpath;
	}
	tst_res(ttype, "group %d got event: mask %llx pid=%u fd=%d filename=%s",
		group, (unsigned long long)event->mask,
		(unsigned int)event->pid, event->fd, filename);
}

const char *event_filename(struct fanotify_event_metadata *event)
{
	struct fanotify_event_info_fid *event_fid;
	struct file_handle *file_handle;
	const char *filename, *end;
	if (event->event_len <= FAN_EVENT_METADATA_LEN)
		return "";
	event_fid = (struct fanotify_event_info_fid *)(event + 1);
	file_handle = (struct file_handle *)event_fid->handle;
	filename = (char *)file_handle->f_handle + file_handle->handle_bytes;
	end = (char *)event_fid + event_fid->hdr.len;
	return (filename == end) ? "" : filename;
}

int  verify_event(int group, struct fanotify_event_metadata *event,
			 uint32_t expect, const char *expect_filename)
{
	const char *filename = event_filename(event);
	if (event->mask != expect) {
		tst_res(TFAIL, "group %d got event: mask %llx (expected %llx) "
			"pid=%u fd=%d filename=%s", group, (unsigned long long)event->mask,
			(unsigned long long)expect,
			(unsigned int)event->pid, event->fd, filename);
	} else if (event->pid != getpid()) {
		tst_res(TFAIL, "group %d got event: mask %llx pid=%u "
			"(expected %u) fd=%d filename=%s", group,
			(unsigned long long)event->mask, (unsigned int)event->pid,
			(unsigned int)getpid(), event->fd, filename);
	} else if (strcmp(filename, expect_filename)) {
		tst_res(TFAIL, "group %d got event: mask %llx pid=%u "
			"fd=%d filename='%s' (expected '%s')", group,
			(unsigned long long)event->mask, (unsigned int)event->pid,
			event->fd, filename, expect_filename);
	} else {
		event_res(TPASS, group, event, filename);
	}
	if (event->fd != FAN_NOFD)
		close(event->fd);
}

void close_event_fds(struct fanotify_event_metadata *event, int buflen)
{
	for (; FAN_EVENT_OK(event, buflen); FAN_EVENT_NEXT(event, buflen)) {
		if (event->fd != FAN_NOFD)
			close(event->fd);
	}
}

void test_fanotify(unsigned int n)
{
	int ret, dirfd;
	unsigned int i;
	struct fanotify_event_metadata *event;
	struct tcase *tc = &tcases[n];
	tst_res(TINFO, "Test #%d: %s", n, tc->tname);
	if (fan_report_dfid_unsupported && tc->report_name) {
		FANOTIFY_INIT_FLAGS_ERR_MSG(FAN_REPORT_DFID_NAME, fan_report_dfid_unsupported);
		return;
	}
	if (tc->ignore && tst_kvercmp(5, 19, 0) < 0) {
		tst_res(TCONF, "ignored mask on parent dir has undefined "
				"behavior on kernel < 5.19");
		return;
	}
	if (ignore_mark_unsupported && tc->ignore & FAN_MARK_IGNORE) {
		tst_res(TCONF, "FAN_MARK_IGNORE not supported in kernel?");
		return;
	}
	create_fanotify_groups(tc);
	 * generate MODIFY event and no FAN_CLOSE_NOWRITE event.
	 */
	fprintf(fname, "1");
	 * generate FAN_CLOSE_NOWRITE event on a child, subdir or "."
	 */
	dirfd = open(tc->event_path, O_RDONLY);
	close(dirfd);
int 	 * First verify the first group got the file MODIFY event and got just
	 * one FAN_CLOSE_NOWRITE event.
	 */
	ret = read(fd_notify[0], event_buf, EVENT_BUF_LEN);
	if (ret < 0) {
		if (errno == EAGAIN) {
			tst_res(TFAIL, "first group did not get event");
		} else {
			tst_brk(TBROK | TERRNO,
				"reading fanotify events failed");
		}
	}
	event = (struct fanotify_event_metadata *)event_buf;
	if (ret < tc->nevents * (int)FAN_EVENT_METADATA_LEN) {
		tst_res(TFAIL,
			"short read when reading fanotify events (%d < %d)",
			ret, tc->nevents * (int)FAN_EVENT_METADATA_LEN);
	}
	if (FAN_EVENT_OK(event, ret)) {
int 		verify_event(0, event, FAN_MODIFY, tc->report_name ? fname : "");
		event = FAN_EVENT_NEXT(event, ret);
	}
	if (tc->nevents > 1 && FAN_EVENT_OK(event, ret)) {
int 		verify_event(0, event, FAN_CLOSE_NOWRITE,
			     tc->report_name ? (tc->ondir ? "." : tc->event_path) : "");
		event = FAN_EVENT_NEXT(event, ret);
	}
	if (ret > 0) {
		tst_res(TFAIL,
			"first group got more than %d events (%d bytes)",
			tc->nevents, ret);
	}
	close_event_fds(event, ret);
int 	 * Then verify the rest of the groups did not get the MODIFY event and
	 * got the FAN_CLOSE_NOWRITE event only on a non-directory.
	 */
	for (i = 1; i < NUM_GROUPS; i++) {
		 * Verify that ignore mask survived the modify event on child,
		 * which was not supposed to be sent to this group.
		 */
		if (tc->ignore)
			check_ignore_mask(fd_notify[i]);
		ret = read(fd_notify[i], event_buf, EVENT_BUF_LEN);
		if (ret > 0) {
			event = (struct fanotify_event_metadata *)event_buf;
int 			verify_event(i, event, tc->nonfirst_event, "");
			event = FAN_EVENT_NEXT(event, ret);
			close_event_fds(event, ret);
			continue;
		}
		if (ret == 0) {
			tst_res(TFAIL, "group %d zero length read from fanotify fd", i);
			continue;
		}
		if (errno != EAGAIN) {
			tst_brk(TBROK | TERRNO,
				"reading fanotify events failed");
		}
		if (tc->nonfirst_event)
			tst_res(TFAIL, "group %d expected and got no event", i);
		else
			tst_res(TPASS, "group %d got no event as expected", i);
	}
	cleanup_fanotify_groups();
}

void setup(void)
{
	fan_report_dfid_unsupported = fanotify_init_flags_supported_on_fs(FAN_REPORT_DFID_NAME,
									  MOUNT_PATH);
	ignore_mark_unsupported = fanotify_mark_supported_by_kernel(FAN_MARK_IGNORE_SURV);
	mkdir(MOUNT_NAME, 0755);
	mount(MOUNT_PATH, MOUNT_NAME, "none", MS_BIND, NULL);
	mount_created = 1;
	chdir(MOUNT_NAME);
	mkdir(DIR_NAME, 0755);
	sprintf(fname, "tfile_%d", getpid());
	fprintf(fname, "1");
	fprintf(FILE2_NAME, "1");
}

void cleanup(void)
{
	cleanup_fanotify_groups();
	chdir("../");
	if (mount_created)
		umount(MOUNT_NAME);
}

void main(){
	setup();
	cleanup();
}
