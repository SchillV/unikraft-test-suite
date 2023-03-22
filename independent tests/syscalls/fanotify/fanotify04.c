#include "incl.h"
#define _GNU_SOURCE
#ifdef HAVE_SYS_FANOTIFY_H
#define EVENT_SIZE  (sizeof(struct fanotify_event_metadata))
#define BUF_SIZE 256
#define TST_TOTAL 9

char fname[BUF_SIZE];

char sname[BUF_SIZE];

char dir[BUF_SIZE];

int fd_notify;

char event_buf[EVENT_SIZE];

char *expect_str_fail(int expect)
{
	if (expect == 0)
		return "failed";
	return "unexpectedly succeeded";
}

char *expect_str_pass(int expect)
{
	if (expect == 0)
		return "succeeded";
	return "failed";
}

void check_mark(char *file, unsigned long long flag, char *flagstr,
		       int expect, void (*test_event)(char *))
{
	if (fanotify_mark(fd_notify, FAN_MARK_ADD | flag, FAN_OPEN, AT_FDCWD,
			  file) != expect) {
		tst_res(TFAIL,
			"fanotify_mark (%d, FAN_MARK_ADD | %s, FAN_OPEN, "
			"AT_FDCWD, '%s') %s", fd_notify, flagstr, file,
			expect_str_fail(expect));
	} else {
		tst_res(TPASS,
			"fanotify_mark (%d, FAN_MARK_ADD | %s, FAN_OPEN, "
			"AT_FDCWD, '%s') %s", fd_notify, flagstr, file,
			expect_str_pass(expect));
		if (expect == -1)
			return;
		if (test_event)
			test_event(file);
		fanotify_mark(fd_notify, FAN_MARK_REMOVE | flag,
				  FAN_OPEN, AT_FDCWD, file);
	}
}
#define CHECK_MARK(file, flag, expect, func) check_mark(file, flag, #flag, expect, func)

void do_open(char *file, int flag)
{
	int fd;
	fd = open(file, O_RDONLY | flag);
	close(fd);
}

void open_file(char *file)
{
	do_open(file, 0);
}

void open_dir(char *file)
{
	do_open(file, O_DIRECTORY);
}

int  verify_event(int mask)
{
	struct fanotify_event_metadata *event;
	struct stat st;
	read(0, fd_notify, event_buf, EVENT_SIZE);
	event = (struct fanotify_event_metadata *)&event_buf;
	if (event->mask != FAN_OPEN) {
		tst_res(TFAIL, "got unexpected event %llx",
			(unsigned long long)event->mask);
	} else if (fstat(event->fd, &st) < 0) {
		tst_res(TFAIL, "failed to stat event->fd (%s)",
			strerror(errno));
	} else if ((int)(st.st_mode & S_IFMT) != mask) {
		tst_res(TFAIL, "event->fd points to object of different type "
			"(%o != %o)", st.st_mode & S_IFMT, mask);
	} else {
		tst_res(TPASS, "event generated properly for type %o", mask);
	}
	if (event->fd != FAN_NOFD)
		close(event->fd);
}

void do_open_test(char *file, int flag, int mask)
{
	do_open(file, flag);
int 	verify_event(mask);
}

void test_open_file(char *file)
{
	do_open_test(file, 0, S_IFREG);
}

int  verify_no_event(
{
	int ret;
	ret = read(fd_notify, event_buf, EVENT_SIZE);
	if (ret != -1) {
		struct fanotify_event_metadata *event;
		event = (struct fanotify_event_metadata *)&event_buf;
		tst_res(TFAIL, "seen unexpected event (mask %llx)",
			(unsigned long long)event->mask);
		if (event->fd != FAN_NOFD)
			close(event->fd);
	} else if (errno != EAGAIN) {
		tst_res(TFAIL | TERRNO, "read(%d, buf, %zu) failed", fd_notify,
			EVENT_SIZE);
	} else {
		tst_res(TPASS, "No event as expected");
	}
}

void test_open_symlink(char *file)
{
	do_open(file, 0);
int 	verify_no_event();
}

void test01(void)
{
	CHECK_MARK(".", FAN_MARK_ONLYDIR, 0, NULL);
	CHECK_MARK(fname, FAN_MARK_ONLYDIR, -1, NULL);
	CHECK_MARK(sname, FAN_MARK_DONT_FOLLOW, 0, test_open_symlink);
	CHECK_MARK(sname, 0, 0, test_open_file);
	fanotify_mark(fd_notify, FAN_MARK_ADD, FAN_OPEN, AT_FDCWD, fname);
	fanotify_mark(fd_notify, FAN_MARK_ADD, FAN_OPEN | FAN_ONDIR,
			   AT_FDCWD, dir);
	open_file(fname);
int 	verify_event(S_IFREG);
	open_dir(dir);
int 	verify_event(S_IFDIR);
	fanotify_mark(fd_notify, FAN_MARK_FLUSH, 0, AT_FDCWD, ".");
	open_dir(dir);
int 	verify_no_event();
}

void setup(void)
{
	int fd;
	sprintf(fname, "fname_%d", getpid());
	fd = open(fname, O_RDWR | O_CREAT, 0644);
	close(fd);
	sprintf(sname, "symlink_%d", getpid());
	symlink(fname, sname);
	sprintf(dir, "dir_%d", getpid());
	mkdir(dir, 0755);
	fd_notify = fanotify_init(FAN_CLASS_NOTIF | FAN_NONBLOCK,
					O_RDONLY);
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
