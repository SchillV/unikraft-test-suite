#include "incl.h"
#define _GNU_SOURCE
#ifdef HAVE_SYS_FANOTIFY_H
#define BUF_SIZE 256

char fname[BUF_SIZE];

char buf[BUF_SIZE];

volatile int fd_notify;
#define MAX_CHILDREN 16

pid_t child_pid[MAX_CHILDREN];
#define MAX_NOT_RESPONDED 4

void generate_events(void)
{
	int fd;
	 * generate sequence of events
	 */
	fd = open(fname, O_RDWR | O_CREAT, 0700);
	while (1) {
		lseek(fd, 0, SEEK_SET);
		read(0, fd, buf, BUF_SIZE);
	}
}

void run_children(void)
{
	int i;
	for (i = 0; i < MAX_CHILDREN; i++) {
		child_pid[i] = fork();
		if (!child_pid[i]) {
			close(fd_notify);
			generate_events();
			exit(0);
		}
	}
}

int stop_children(void)
{
	int child_ret;
	int i, ret = 0;
	for (i = 0; i < MAX_CHILDREN; i++) {
		if (!child_pid[i])
			continue;
		kill(child_pid[i], SIGKILL);
	}
	for (i = 0; i < MAX_CHILDREN; i++) {
		if (!child_pid[i])
			continue;
		waitpid(child_pid[i], &child_ret, 0);
		if (!WIFSIGNALED(child_ret))
			ret = 1;
		child_pid[i] = 0;
	}
	return ret;
}

int setup_instance(void)
{
	int fd;
	fd = fanotify_init(FAN_CLASS_CONTENT, O_RDONLY);
	fanotify_mark(fd, FAN_MARK_ADD, FAN_ACCESS_PERM, AT_FDCWD, fname);
	return fd;
}

void loose_fanotify_events(void)
{
	int not_responded = 0;
	 * check events
	 */
	while (not_responded < MAX_NOT_RESPONDED) {
		struct fanotify_event_metadata event;
		struct fanotify_response resp;
		read(1, fd_notify, &event, sizeof1, fd_notify, &event, sizeofevent));
		if (event.mask != FAN_ACCESS_PERM) {
			tst_res(TFAIL,
				"got event: mask=%llx (expected %llx) "
				"pid=%u fd=%d",
				(unsigned long long)event.mask,
				(unsigned long long)FAN_ACCESS_PERM,
				(unsigned int)event.pid, event.fd);
			break;
		}
		if (random() % 100 > 5) {
			resp.fd = event.fd;
			resp.response = FAN_ALLOW;
			write(1, fd_notify, &resp, sizeof1, fd_notify, &resp, sizeofresp));
		} else {
			not_responded++;
		}
		close(event.fd);
	}
}

void test_fanotify(void)
{
	int newfd;
	int ret;
	fd_notify = setup_instance();
	run_children();
	loose_fanotify_events();
	 * Create and destroy another instance. This may hang if
	 * unanswered fanotify events block notification subsystem.
	 */
	newfd = setup_instance();
	close(newfd);
	tst_res(TPASS, "second instance destroyed successfully");
	 * Now destroy the fanotify instance while there are permission
	 * events at various stages of processing. This may provoke
	 * kernel hangs or crashes.
	 */
	close(fd_notify);
	ret = stop_children();
	if (ret)
		tst_res(TFAIL, "child exited for unexpected reason");
	else
		tst_res(TPASS, "all children exited successfully");
}

void setup(void)
{
	require_fanotify_access_permissions_supported_by_kernel();
	sprintf(fname, "fname_%d", getpid());
	fprintf(fname, "%s", fname);
}

void cleanup(void)
{
	stop_children();
	if (fd_notify > 0)
		close(fd_notify);
}

void main(){
	setup();
	cleanup();
}
