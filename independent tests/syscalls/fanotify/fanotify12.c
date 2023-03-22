#include "incl.h"
#define _GNU_SOURCE
#ifdef HAVE_SYS_FANOTIFY_H
#define EVENT_MAX 1024
#define EVENT_SIZE (sizeof(struct fanotify_event_metadata))
#define EVENT_BUF_LEN (EVENT_MAX * EVENT_SIZE)
#define EVENT_SET_BUF 32
#define BUF_SIZE 256
#define TEST_APP "fanotify_child"

pid_t child_pid;

char fname[BUF_SIZE];

volatile int fd_notify;

volatile int complete;

char event_buf[EVENT_BUF_LEN];

int exec_events_unsupported;

struct test_case_t {
	const char *tname;
	struct fanotify_mark_type mark;
	unsigned long long mask;
	unsigned long long ignore_mask;
	int event_count;
	unsigned long long event_set[EVENT_SET_BUF];
} test_cases[] = {
	{
		"inode mark, FAN_OPEN events",
		INIT_FANOTIFY_MARK_TYPE(INODE),
		FAN_OPEN,
		0,
		2,
		{FAN_OPEN, FAN_OPEN}
	},
	{
		"inode mark, FAN_OPEN_EXEC events",
		INIT_FANOTIFY_MARK_TYPE(INODE),
		FAN_OPEN_EXEC,
		0,
		1,
		{FAN_OPEN_EXEC}
	},
	{
		"inode mark, FAN_OPEN | FAN_OPEN_EXEC events",
		INIT_FANOTIFY_MARK_TYPE(INODE),
		FAN_OPEN | FAN_OPEN_EXEC,
		0,
		2,
		{FAN_OPEN, FAN_OPEN | FAN_OPEN_EXEC}
	},
	{
		"inode mark, FAN_OPEN events, ignore FAN_OPEN_EXEC",
		INIT_FANOTIFY_MARK_TYPE(INODE),
		FAN_OPEN,
		FAN_OPEN_EXEC,
		2,
		{FAN_OPEN, FAN_OPEN}
	},
	{
		"inode mark, FAN_OPEN_EXEC events, ignore FAN_OPEN",
		INIT_FANOTIFY_MARK_TYPE(INODE),
		FAN_OPEN_EXEC,
		FAN_OPEN,
		1,
		{FAN_OPEN_EXEC}
	},
	{
		"inode mark, FAN_OPEN | FAN_OPEN_EXEC events, ignore "
		"FAN_OPEN_EXEC",
		INIT_FANOTIFY_MARK_TYPE(INODE),
		FAN_OPEN | FAN_OPEN_EXEC,
		FAN_OPEN_EXEC,
		2,
		{FAN_OPEN, FAN_OPEN}
	}
};

int generate_events(void)
{
	int fd, status;
	child_pid = fork();
	 * Generate a sequence of events
	 */
	if (child_pid == 0) {
		close(fd_notify);
		fd = open(fname, O_RDONLY);
		if (fd > 0)
			close(fd);
		execl(TEST_APP, TEST_APP, NULL);
		exit(1);
	}
	waitpid(child_pid, &status, 0);
	if (WIFEXITED(status) && WEXITSTATUS(status) == 0)
		return 1;
	return 0;
}

int setup_mark(unsigned int n)
{
	unsigned int i = 0;
	struct test_case_t *tc = &test_cases[n];
	struct fanotify_mark_type *mark = &tc->mark;
	const char *const files[] = {fname, TEST_APP};
	tst_res(TINFO, "Test #%d: %s", n, tc->tname);
	if (exec_events_unsupported && ((tc->mask & FAN_OPEN_EXEC) ||
					tc->ignore_mask & FAN_OPEN_EXEC)) {
		tst_res(TCONF, "FAN_OPEN_EXEC not supported in kernel?");
		return -1;
	}
	fd_notify = fanotify_init(FAN_CLASS_NOTIF, O_RDONLY);
	for (; i < ARRAY_SIZE(files); i++) {
		fanotify_mark(fd_notify, FAN_MARK_ADD | mark->flag,
					tc->mask, AT_FDCWD, files[i]);
		if (tc->ignore_mask) {
			fanotify_mark(fd_notify, FAN_MARK_ADD | mark->flag
						| FAN_MARK_IGNORED_MASK,
						tc->ignore_mask, AT_FDCWD,
						files[i]);
		}
	}
	return 0;
}

void do_test(unsigned int n)
{
	int len = 0, event_num = 0;
	struct test_case_t *tc = &test_cases[n];
	struct fanotify_event_metadata *event;
	if (setup_mark(n) != 0)
		goto cleanup;
	if (!generate_events())
		goto cleanup;
	len = read(0, fd_notify, event_buf, EVENT_BUF_LEN);
	event = (struct fanotify_event_metadata *) event_buf;
	while (FAN_EVENT_OK(event, len) && event_num < tc->event_count) {
		if (event->mask != *(tc->event_set + event_num)) {
			tst_res(TFAIL,
				"Received event: mask=%llx (expected %llx, "
				"pid=%u, fd=%d",
				(unsigned long long) event->mask,
				*(tc->event_set + event_num),
				(unsigned int) event->pid,
				event->fd);
		} else if (event->pid != child_pid) {
			tst_res(TFAIL,
				"Received event: mask=%llx, pid=%u (expected "
				"%u), fd=%d",
				(unsigned long long) event->mask,
				(unsigned int) event->pid,
				(unsigned int) child_pid,
				event->fd);
		} else {
			tst_res(TPASS,
				"Received event: mask=%llx, pid=%u, fd=%d",
				(unsigned long long) event->mask,
				(unsigned int) event->pid,
				event->fd);
		}
		if (event->fd != FAN_NOFD)
			close(event->fd);
		event_num++;
		event = FAN_EVENT_NEXT(event, len);
	}
cleanup:
	if (fd_notify > 0)
		close(fd_notify);
}

void do_setup(void)
{
	exec_events_unsupported = fanotify_events_supported_by_kernel(FAN_OPEN_EXEC,
								      FAN_CLASS_NOTIF, 0);
	sprintf(fname, "fname_%d", getpid());
	fprintf(fname, "1");
}

void do_cleanup(void)
{
	if (fd_notify > 0)
		close(fd_notify);
}

void main(){
	setup();
	cleanup();
}
