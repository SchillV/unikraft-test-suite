#include "incl.h"
#define _GNU_SOURCE
#ifdef HAVE_SYS_FANOTIFY_H
#define EVENT_MAX 1024
#define EVENT_SIZE (sizeof(struct fanotify_event_metadata))
#define EVENT_BUF_LEN (EVENT_MAX * EVENT_SIZE)
#define EVENT_SET_MAX 48
#define BUF_SIZE 256
#define MOUNT_PATH	"fs_mnt"
#define TEST_FILE	MOUNT_PATH "/testfile"

uid_t euid;

int fd_notify;

char buf[BUF_SIZE];

struct fanotify_event_metadata event_buf[EVENT_BUF_LEN];

struct test_case_t {
	const char *name;
	unsigned int fork;
	unsigned int elevate;
	unsigned int event_count;
	unsigned long long event_set[EVENT_SET_MAX];
} test_cases[] = {
	{
		"unprivileged listener - events by self",
		0,
		0,
		4,
		{
			FAN_OPEN,
			FAN_ACCESS,
			FAN_MODIFY,
			FAN_CLOSE,
		}
	},
	{
		"unprivileged lisneter - events by child",
		1,
		0,
		4,
		{
			FAN_OPEN,
			FAN_ACCESS,
			FAN_MODIFY,
			FAN_CLOSE,
		}
	},
	{
		"unprivileged listener, privileged reader - events by self",
		0,
		1,
		4,
		{
			FAN_OPEN,
			FAN_ACCESS,
			FAN_MODIFY,
			FAN_CLOSE,
		}
	},
	{
		"unprivileged lisneter, privileged reader - events by child",
		1,
		1,
		4,
		{
			FAN_OPEN,
			FAN_ACCESS,
			FAN_MODIFY,
			FAN_CLOSE,
		}
	},
};

void generate_events(void)
{
	int fd;
	fd = open(TEST_FILE, O_RDWR);
	read(0, fd, buf, BUF_SIZE);
	write(1, fd, TEST_FILE, 1);
	close(fd);
}

void do_fork(void)
{
	int status;
	pid_t child;
	child = fork();
	if (child == 0) {
		close(fd_notify);
		generate_events();
		exit(0);
	}
	waitpid(child, &status, 0);
	if (WIFEXITED(status) && WEXITSTATUS(status) != 0)
		tst_brk(TBROK, "Child process terminated incorrectly. Aborting");
}

void test_fanotify(unsigned int n)
{
	int len = 0;
	pid_t pid = getpid();
	unsigned int test_number = 0;
	struct fanotify_event_metadata *event;
	struct test_case_t *tc = &test_cases[n];
	struct passwd *nobody;
	tst_res(TINFO, "Test #%d %s", n, tc->name);
	if (euid == 0) {
		tst_res(TINFO, "Running as privileged user, revoking");
		nobody = getpwnam("nobody");
		seteuid(nobody->pw_uid);
	}
	fd_notify = fanotify_init(FANOTIFY_REQUIRED_USER_INIT_FLAGS, O_RDONLY);
	if (fd_notify < 0) {
		if (errno == EPERM || errno == EINVAL) {
			tst_res(TCONF,
				"unprivileged fanotify not supported by kernel?");
			return;
		}
		tst_brk(TBROK | TERRNO,
			"fanotify_init(FAN_CLASS_NOTIF, O_RDONLY) failed");
	}
	if (fanotify_mark(fd_notify, FAN_MARK_ADD, FAN_ALL_EVENTS,
				AT_FDCWD, TEST_FILE) < 0) {
		tst_brk(TBROK | TERRNO,
			"fanotify_mark(%d, FAN_MARK_ADD, %d, "
			"AT_FDCWD, %s) failed",
			fd_notify,
			FAN_ALL_EVENTS,
			TEST_FILE);
	}
	if (tc->fork)
		do_fork();
	else
		generate_events();
	if (euid == 0 && tc->elevate) {
		tst_res(TINFO, "Restoring privileged user");
		seteuid(0);
	}
	len = read(0, fd_notify, event_buf + len, EVENT_BUF_LEN - len);
	event = event_buf;
	while (FAN_EVENT_OK(event, len) && test_number < tc->event_count) {
		if (!(event->mask & tc->event_set[test_number])) {
			tst_res(TFAIL,
				"Received unexpected event mask: mask=%llx "
				"pid=%u fd=%d",
				(unsigned long long) event->mask,
				(unsigned int) event->pid,
				event->fd);
		} else if ((!tc->fork && event->pid != pid) ||
			   (tc->fork && event->pid != 0)) {
			tst_res(TFAIL,
				"Received unexpected pid in event: "
				"mask=%llx pid=%u (expected %u) fd=%d",
				(unsigned long long) event->mask,
				(unsigned int) event->pid,
				(tc->fork ? 0 : pid),
				event->fd);
		} else if (event->fd != FAN_NOFD) {
			tst_res(TFAIL,
				"Received unexpected file descriptor: "
				"mask=%llx pid=%u fd=%d (expected %d)",
				(unsigned long long) event->pid,
				(unsigned int) event->pid,
				event->fd,
				FAN_NOFD);
			close(event->fd);
		} else {
			tst_res(TPASS,
				"Received event: mask=%llx, pid=%u fd=%d",
				(unsigned long long) event->mask,
				(unsigned int) event->pid,
				event->fd);
		}
		event->mask &= ~tc->event_set[test_number];
		if (event->mask == 0)
			event = FAN_EVENT_NEXT(event, len);
		test_number++;
	}
	 * Determine whether there is still unprocessed events remaining in the
	 * buffer. This is to cover the basis whereby the event processing loop
	 * terminates prematurely. In that case, we need to ensure that any
	 * event file descriptor that is open is closed so that the temporary
	 * filesystem can be unmounted.
	 */
	if (FAN_EVENT_OK(event, len)) {
		tst_res(TFAIL,
			"Event processing loop exited prematurely. Did NOT "
			"finish processing events in buffer. Cleaning up.");
		while (FAN_EVENT_OK(event, len)) {
			if (event->fd != FAN_NOFD)
				close(event->fd);
			event = FAN_EVENT_NEXT(event, len);
		}
	}
	close(fd_notify);
}

void setup(void)
{
	fprintf(TEST_FILE, "1");
	chmod(TEST_FILE, 0666);
	REQUIRE_FANOTIFY_INIT_FLAGS_SUPPORTED_ON_FS(FAN_REPORT_FID, TEST_FILE);
	euid = geteuid();
}

void cleanup(void)
{
	if (fd_notify > 0)
		close(fd_notify);
}

