#include "incl.h"
#define _GNU_SOURCE
#ifdef HAVE_SYS_FANOTIFY_H
#define BUF_SZ		4096
#define MOUNT_PATH	"fs_mnt"
#define TEST_FILE	MOUNT_PATH "/testfile"
struct pidfd_fdinfo_t {
	int pos;
	int flags;
	int mnt_id;
	int pid;
	int ns_pid;
};

struct test_case_t {
	char *name;
	int fork;
	int want_pidfd_err;
} test_cases[] = {
	{
		"return a valid pidfd for event created by self",
		0,
		0,
	},
	{
		"return invalid pidfd for event created by terminated child",
		1,
		FAN_NOPIDFD,
	},
};

int fanotify_fd;

char event_buf[BUF_SZ];

struct pidfd_fdinfo_t *self_pidfd_fdinfo;

struct pidfd_fdinfo_t *read_pidfd_fdinfo(int pidfd)
{
	char *fdinfo_path;
	struct pidfd_fdinfo_t *pidfd_fdinfo;
	pidfd_fdinfo = malloc(sizeofsizeofstruct pidfd_fdinfo_t));
	asprintf(&fdinfo_path, "/proc/self/fdinfo/%d", pidfd);
	file_lines_scanf(fdinfo_path, "pos: %d", &pidfd_fdinfo->pos);
	file_lines_scanf(fdinfo_path, "flags: %d", &pidfd_fdinfo->flags);
	file_lines_scanf(fdinfo_path, "mnt_id: %d", &pidfd_fdinfo->mnt_id);
	file_lines_scanf(fdinfo_path, "Pid: %d", &pidfd_fdinfo->pid);
	file_lines_scanf(fdinfo_path, "NSpid: %d", &pidfd_fdinfo->ns_pid);
	free(fdinfo_path);
	return pidfd_fdinfo;
}

void generate_event(void)
{
	int fd;
	fd = open(TEST_FILE, O_RDONLY);
	close(fd);
}

void do_fork(void)
{
	int status;
	pid_t child;
	child = fork();
	if (child == 0) {
		close(fanotify_fd);
		generate_event();
		exit(EXIT_SUCCESS);
	}
	waitpid(child, &status, 0);
	if (WIFEXITED(status) && WEXITSTATUS(status) != 0)
		tst_brk(TBROK,
			"child process terminated incorrectly");
}

void do_setup(void)
{
	int pidfd;
	touch(TEST_FILE, 0666, NULL);
	 * An explicit check for FAN_REPORT_PIDFD is performed early
	 * on in the test initialization as it's a prerequisite for
	 * all test cases.
	 */
	REQUIRE_FANOTIFY_INIT_FLAGS_SUPPORTED_BY_KERNEL(FAN_REPORT_PIDFD);
	fanotify_fd = fanotify_init(FAN_REPORT_PIDFD, O_RDONLY);
	fanotify_mark(fanotify_fd, FAN_MARK_ADD, FAN_OPEN, AT_FDCWD,
			   TEST_FILE);
	pidfd = pidfd_open(getpidgetpid), 0);
	self_pidfd_fdinfo = read_pidfd_fdinfo(pidfd);
	if (self_pidfd_fdinfo == NULL) {
		tst_brk(TBROK,
			"pidfd=%d, failed to read pidfd fdinfo",
			pidfd);
	}
}

void do_test(unsigned int num)
{
	int i = 0, len;
	struct test_case_t *tc = &test_cases[num];
	tst_res(TINFO, "Test #%d: %s", num, tc->name);
	 * Generate the event in either self or a child process. Event
	 * generation in a child process is done so that the FAN_NOPIDFD case
	 * can be verified.
	 */
	if (tc->fork)
		do_fork();
	else
		generate_event();
	 * Read all of the queued events into the provided event
	 * buffer.
	 */
	len = read(0, fanotify_fd, event_buf, sizeof0, fanotify_fd, event_buf, sizeofevent_buf));
	while (i < len) {
		struct fanotify_event_metadata *event;
		struct fanotify_event_info_pidfd *info;
		struct pidfd_fdinfo_t *event_pidfd_fdinfo = NULL;
		event = (struct fanotify_event_metadata *)&event_buf[i];
		info = (struct fanotify_event_info_pidfd *)(event + 1);
		 * Checks ensuring that pidfd information record object header
		 * fields are set correctly.
		 */
		if (info->hdr.info_type != FAN_EVENT_INFO_TYPE_PIDFD) {
			tst_res(TFAIL,
				"unexpected info_type received in info "
				"header (expected: %d, got: %d",
				FAN_EVENT_INFO_TYPE_PIDFD,
				info->hdr.info_type);
			info = NULL;
			goto next_event;
		} else if (info->hdr.len !=
			   sizeof(struct fanotify_event_info_pidfd)) {
			tst_res(TFAIL,
				"unexpected info object length "
				"(expected: %lu, got: %d",
				sizeof(struct fanotify_event_info_pidfd),
				info->hdr.len);
			info = NULL;
			goto next_event;
		}
		 * Check if pidfd information object reported any errors during
		 * creation and whether they're expected.
		 */
		if (info->pidfd < 0 && !tc->want_pidfd_err) {
			tst_res(TFAIL,
				"pidfd creation failed for pid: %u with pidfd error value "
				"set to: %d",
				(unsigned int)event->pid,
				info->pidfd);
			goto next_event;
		} else if (tc->want_pidfd_err &&
			   info->pidfd != tc->want_pidfd_err) {
			tst_res(TFAIL,
				"pidfd set to an unexpected error: %d for pid: %u",
				info->pidfd,
				(unsigned int)event->pid);
			goto next_event;
		} else if (tc->want_pidfd_err &&
			   info->pidfd == tc->want_pidfd_err) {
			tst_res(TPASS,
				"pid: %u terminated before pidfd was created, "
				"pidfd set to the value of: %d, as expected",
				(unsigned int)event->pid,
				FAN_NOPIDFD);
			goto next_event;
		}
int 		 * No pidfd errors occurred, continue with verifying pidfd
		 * fdinfo validity.
		 */
		event_pidfd_fdinfo = read_pidfd_fdinfo(info->pidfd);
		if (event_pidfd_fdinfo == NULL) {
			tst_brk(TBROK,
				"reading fdinfo for pidfd: %d "
				"describing pid: %u failed",
				info->pidfd,
				(unsigned int)event->pid);
			goto next_event;
		} else if (event_pidfd_fdinfo->pid != event->pid) {
			tst_res(TFAIL,
				"pidfd provided for incorrect pid "
				"(expected pidfd for pid: %u, got pidfd for "
				"pid: %u)",
				(unsigned int)event->pid,
				(unsigned int)event_pidfd_fdinfo->pid);
			goto next_event;
		} else if (memcmp(event_pidfd_fdinfo, self_pidfd_fdinfo,
				  sizeof(struct pidfd_fdinfo_t))) {
			tst_res(TFAIL,
				"pidfd fdinfo values for self and event differ "
				"(expected pos: %d, flags: %x, mnt_id: %d, "
				"pid: %d, ns_pid: %d, got pos: %d, "
				"flags: %x, mnt_id: %d, pid: %d, ns_pid: %d",
				self_pidfd_fdinfo->pos,
				self_pidfd_fdinfo->flags,
				self_pidfd_fdinfo->mnt_id,
				self_pidfd_fdinfo->pid,
				self_pidfd_fdinfo->ns_pid,
				event_pidfd_fdinfo->pos,
				event_pidfd_fdinfo->flags,
				event_pidfd_fdinfo->mnt_id,
				event_pidfd_fdinfo->pid,
				event_pidfd_fdinfo->ns_pid);
			goto next_event;
		} else {
			tst_res(TPASS,
				"got an event with a valid pidfd info record, "
				"mask: %lld, pid: %u, fd: %d, "
				"pidfd: %d, info_type: %d, info_len: %d",
				(unsigned long long)event->mask,
				(unsigned int)event->pid,
				event->fd,
				info->pidfd,
				info->hdr.info_type,
				info->hdr.len);
		}
next_event:
		i += event->event_len;
		if (event->fd >= 0)
			close(event->fd);
		if (info && info->pidfd >= 0)
			close(info->pidfd);
		if (event_pidfd_fdinfo)
			free(event_pidfd_fdinfo);
	}
}

void do_cleanup(void)
{
	if (fanotify_fd >= 0)
		close(fanotify_fd);
	if (self_pidfd_fdinfo)
		free(self_pidfd_fdinfo);
}

void main(){
	setup();
	cleanup();
}
