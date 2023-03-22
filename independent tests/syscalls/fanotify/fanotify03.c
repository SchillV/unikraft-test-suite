#include "incl.h"
#define _GNU_SOURCE
#ifdef HAVE_SYS_FANOTIFY_H
# include "fanotify.h"
#define EVENT_MAX 1024
#define EVENT_SIZE  (sizeof(struct fanotify_event_metadata))
#define EVENT_BUF_LEN        (EVENT_MAX * EVENT_SIZE)
#define EVENT_SET_MAX 16
#define BUF_SIZE 256
#define TST_TOTAL 3
#define TEST_APP "fanotify_child"
#define MOUNT_PATH "fs_mnt"
#define FILE_EXEC_PATH MOUNT_PATH"/"TEST_APP

char fname[BUF_SIZE];

char buf[BUF_SIZE];

volatile int fd_notify;

pid_t child_pid;

char event_buf[EVENT_BUF_LEN];

int exec_events_unsupported;

int filesystem_mark_unsupported;
struct event {
	unsigned long long mask;
	unsigned int response;
};

struct tcase {
	const char *tname;
	struct fanotify_mark_type mark;
	unsigned long long mask;
	int event_count;
	struct event event_set[EVENT_SET_MAX];
} tcases[] = {
	{
		"inode mark, FAN_OPEN_PERM | FAN_ACCESS_PERM events",
		INIT_FANOTIFY_MARK_TYPE(INODE),
		FAN_OPEN_PERM | FAN_ACCESS_PERM, 3,
		{
			{FAN_OPEN_PERM, FAN_ALLOW},
			{FAN_ACCESS_PERM, FAN_DENY},
			{FAN_OPEN_PERM, FAN_DENY}
		}
	},
	{
		"inode mark, FAN_ACCESS_PERM | FAN_OPEN_EXEC_PERM events",
		INIT_FANOTIFY_MARK_TYPE(INODE),
		FAN_ACCESS_PERM | FAN_OPEN_EXEC_PERM, 2,
		{
			{FAN_ACCESS_PERM, FAN_DENY},
			{FAN_OPEN_EXEC_PERM, FAN_DENY}
		}
	},
	{
		"mount mark, FAN_OPEN_PERM | FAN_ACCESS_PERM events",
		INIT_FANOTIFY_MARK_TYPE(MOUNT),
		FAN_OPEN_PERM | FAN_ACCESS_PERM, 3,
		{
			{FAN_OPEN_PERM, FAN_ALLOW},
			{FAN_ACCESS_PERM, FAN_DENY},
			{FAN_OPEN_PERM, FAN_DENY}
		}
	},
	{
		"mount mark, FAN_ACCESS_PERM | FAN_OPEN_EXEC_PERM events",
		INIT_FANOTIFY_MARK_TYPE(MOUNT),
		FAN_ACCESS_PERM | FAN_OPEN_EXEC_PERM, 2,
		{
			{FAN_ACCESS_PERM, FAN_DENY},
			{FAN_OPEN_EXEC_PERM, FAN_DENY}
		}
	},
	{
		"filesystem mark, FAN_OPEN_PERM | FAN_ACCESS_PERM events",
		INIT_FANOTIFY_MARK_TYPE(FILESYSTEM),
		FAN_OPEN_PERM | FAN_ACCESS_PERM, 3,
		{
			{FAN_OPEN_PERM, FAN_ALLOW},
			{FAN_ACCESS_PERM, FAN_DENY},
			{FAN_OPEN_PERM, FAN_DENY}
		}
	},
	{
		"filesystem mark, FAN_ACCESS_PERM | FAN_OPEN_EXEC_PERM events",
		INIT_FANOTIFY_MARK_TYPE(FILESYSTEM),
		FAN_ACCESS_PERM | FAN_OPEN_EXEC_PERM, 2,
		{
			{FAN_ACCESS_PERM, FAN_DENY},
			{FAN_OPEN_EXEC_PERM, FAN_DENY}
		}
	},
};

void generate_events(void)
{
	int fd;
	char *const argv[] = {FILE_EXEC_PATH, NULL};
	 * Generate sequence of events
	 */
	fd = open(fname, O_RDWR | O_CREAT, 0700);
	write(
	lseek(fd, 0, SEEK_SET);
	if (read(fd, buf, BUF_SIZE) != -1)
		exit(3);
	close(fd);
	if (execve(FILE_EXEC_PATH, argv, environ) != -1)
		exit(5);
}

void child_handler(int tmp)
{
	(void)tmp;
	 * Close notification fd so that we cannot block while reading
	 * from it
	 */
	close(fd_notify);
	fd_notify = -1;
}

void run_child(void)
{
	struct sigaction child_action;
	child_action.sa_handler = child_handler;
	sigemptyset(&child_action.sa_mask);
	child_action.sa_flags = SA_NOCLDSTOP;
	if (sigaction(SIGCHLD, &child_action, NULL) < 0) {
		tst_brk(TBROK | TERRNO,
			"sigaction(SIGCHLD, &child_action, NULL) failed");
	}
	child_pid = fork();
	if (child_pid == 0) {
		close(fd_notify);
		generate_events();
		exit(0);
	}
}

void check_child(void)
{
	struct sigaction child_action;
	int child_ret;
	child_action.sa_handler = SIG_IGN;
	sigemptyset(&child_action.sa_mask);
	child_action.sa_flags = SA_NOCLDSTOP;
	if (sigaction(SIGCHLD, &child_action, NULL) < 0) {
		tst_brk(TBROK | TERRNO,
			"sigaction(SIGCHLD, &child_action, NULL) failed");
	}
	waitpid(-1, &child_ret, 0);
	if (WIFEXITED(child_ret) && WEXITSTATUS(child_ret) == 0)
		tst_res(TPASS, "child exited correctly");
	else
		tst_res(TFAIL, "child %s", tst_strstatus(child_ret));
}

int setup_mark(unsigned int n)
{
	unsigned int i = 0;
	struct tcase *tc = &tcases[n];
	struct fanotify_mark_type *mark = &tc->mark;
	char *const files[] = {fname, FILE_EXEC_PATH};
	tst_res(TINFO, "Test #%d: %s", n, tc->tname);
	if (exec_events_unsupported && tc->mask & FAN_OPEN_EXEC_PERM) {
		tst_res(TCONF, "FAN_OPEN_EXEC_PERM not supported in kernel?");
		return -1;
	}
	if (filesystem_mark_unsupported && mark->flag == FAN_MARK_FILESYSTEM) {
		tst_res(TCONF, "FAN_MARK_FILESYSTEM not supported in kernel?");
		return -1;
	}
	fd_notify = fanotify_init(FAN_CLASS_CONTENT, O_RDONLY);
	for (; i < ARRAY_SIZE(files); i++) {
		fanotify_mark(fd_notify, FAN_MARK_ADD | mark->flag,
				  tc->mask, AT_FDCWD, files[i]);
	}
	return 0;
}

void test_fanotify(unsigned int n)
{
	int ret, len = 0, i = 0, test_num = 0;
	struct tcase *tc = &tcases[n];
	struct event *event_set = tc->event_set;
	if (setup_mark(n) != 0)
		return;
	run_child();
	 * Process events
	 *
	 * tc->count + 1 is to accommodate for checking the child process
	 * return value
	 */
	while (test_num < tc->event_count + 1 && fd_notify != -1) {
		struct fanotify_event_metadata *event;
		if (i == len) {
			ret = read(fd_notify, event_buf + len,
				   EVENT_BUF_LEN - len);
			if (fd_notify == -1)
				break;
			if (ret < 0) {
				tst_brk(TBROK,
					"read(%d, buf, %zu) failed",
					fd_notify, EVENT_BUF_LEN);
			}
			len += ret;
		}
		event = (struct fanotify_event_metadata *)&event_buf[i];
		 * reported should exactly match the event mask within the
		 * event set.
		 */
		if (event->mask != event_set[test_num].mask) {
			tst_res(TFAIL,
				"got event: mask=%llx (expected %llx) "
				"pid=%u fd=%d",
				(unsigned long long)event->mask,
				event_set[test_num].mask,
				(unsigned int)event->pid, event->fd);
		} else if (event->pid != child_pid) {
			tst_res(TFAIL,
				"got event: mask=%llx pid=%u "
				"(expected %u) fd=%d",
				(unsigned long long)event->mask,
				(unsigned int)event->pid,
				(unsigned int)child_pid,
				event->fd);
		} else {
			tst_res(TPASS,
				"got event: mask=%llx pid=%u fd=%d",
				(unsigned long long)event->mask,
				(unsigned int)event->pid, event->fd);
		}
		if (event_set[test_num].mask & LTP_ALL_PERM_EVENTS) {
			struct fanotify_response resp;
			resp.fd = event->fd;
			resp.response = event_set[test_num].response;
			write(1, fd_notify, &resp, sizeof1, fd_notify, &resp, sizeofresp));
		}
		i += event->event_len;
		if (event->fd != FAN_NOFD)
			close(event->fd);
		test_num++;
	}
	for (; test_num < tc->event_count; test_num++) {
		tst_res(TFAIL, "didn't get event: mask=%llx",
			event_set[test_num].mask);
	}
	check_child();
	if (fd_notify > 0)
		close(fd_notify);
}

void setup(void)
{
	require_fanotify_access_permissions_supported_by_kernel();
	filesystem_mark_unsupported = fanotify_mark_supported_by_kernel(FAN_MARK_FILESYSTEM);
	exec_events_unsupported = fanotify_events_supported_by_kernel(FAN_OPEN_EXEC_PERM,
								      FAN_CLASS_CONTENT, 0);
	sprintf(fname, MOUNT_PATH"/fname_%d", getpid());
	fprintf(fname, "1");
	cp(TEST_APP, FILE_EXEC_PATH);
}

void cleanup(void)
{
	if (fd_notify > 0)
		close(fd_notify);
}

const char *const resource_files[] = {
	TEST_APP,
	NULL
};

void main(){
	setup();
	cleanup();
}
