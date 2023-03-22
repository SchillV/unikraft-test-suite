#include "incl.h"
#if defined(HAVE_SYS_INOTIFY_H)
# include <sys/inotify.h>
#endif
#if defined(HAVE_SYS_INOTIFY_H)
#define EVENT_MAX 1024
#define EVENT_SIZE  (sizeof(struct inotify_event))
#define EVENT_BUF_LEN        (EVENT_MAX * (EVENT_SIZE + 16))
#define BUF_SIZE 256
struct event_t {
	char name[BUF_SIZE];
	unsigned int mask;
};
#define	TEST_DIR	"test_dir"
#define	TEST_FILE	"test_file"
struct event_t event_set[EVENT_MAX];
char event_buf[EVENT_BUF_LEN];
int fd_notify, reap_wd_file, reap_wd_dir, wd_dir, wd_file;

void cleanup(void)
{
	if (reap_wd_dir && myinotify_rm_watch(fd_notify, wd_dir) == -1)
		tst_res(TWARN,
			"inotify_rm_watch(%d, %d) [1] failed", fd_notify,
			wd_dir);
	if (reap_wd_file && myinotify_rm_watch(fd_notify, wd_file) == -1)
		tst_res(TWARN,
			"inotify_rm_watch(%d, %d) [2] failed", fd_notify,
			wd_file);
	if (fd_notify > 0)
		close(fd_notify);
}

void setup(void)
{
	fd_notify = myinotify_init();
}
int  verify_inotify(
{
	int i = 0, test_num = 0, len;
	int test_cnt = 0;
	mkdir(TEST_DIR, 00700);
	close(creat(TEST_FILE, 00600));
	wd_dir = myinotify_add_watch(fd_notify, TEST_DIR, IN_ALL_EVENTS);
	reap_wd_dir = 1;
	wd_file = myinotify_add_watch(fd_notify, TEST_FILE, IN_ALL_EVENTS);
	reap_wd_file = 1;
	rmdir(TEST_DIR);
	reap_wd_dir = 0;
	event_set[test_cnt].mask = IN_DELETE_SELF;
	strcpy(event_set[test_cnt].name, "");
	test_cnt++;
	event_set[test_cnt].mask = IN_IGNORED;
	strcpy(event_set[test_cnt].name, "");
	test_cnt++;
	unlink(TEST_FILE);
	reap_wd_file = 0;
	 * When a file is unlinked, the link count is reduced by 1, and when it
	 * hits 0 the file is removed.
	 *
	 * This isn't well documented in inotify(7), but it's intuitive if you
	 * understand how Unix works.
	 */
	event_set[test_cnt].mask = IN_ATTRIB;
	strcpy(event_set[test_cnt].name, "");
	test_cnt++;
	event_set[test_cnt].mask = IN_DELETE_SELF;
	strcpy(event_set[test_cnt].name, TEST_FILE);
	test_cnt++;
	event_set[test_cnt].mask = IN_IGNORED;
	strcpy(event_set[test_cnt].name, "");
	test_cnt++;
	len = read(fd_notify, event_buf, EVENT_BUF_LEN);
	if (len == -1)
		tst_brk(TBROK | TERRNO, "read failed");
	while (i < len) {
		struct inotify_event *event;
		event = (struct inotify_event *)&event_buf[i];
		if (test_num >= test_cnt) {
			tst_res(TFAIL,
				"got unnecessary event: "
				"wd=%d mask=%04x cookie=%u len=%u "
				"name=\"%.*s\"", event->wd, event->mask,
				event->cookie, event->len, event->len, event->name);
		} else if ((event_set[test_num].mask == event->mask)
			   &&
			   (!strncmp
			    (event_set[test_num].name, event->name,
			     event->len))) {
			tst_res(TPASS,
				"got event: wd=%d mask=%04x "
				"cookie=%u len=%u name=\"%.*s\"",
				event->wd, event->mask, event->cookie,
				event->len, event->len, event->name);
		} else {
			tst_res(TFAIL, "got event: wd=%d mask=%04x "
				"(expected %x) cookie=%u len=%u "
				"name=\"%.*s\" (expected \"%s\") %d",
				event->wd, event->mask,
				event_set[test_num].mask,
				event->cookie, event->len,
				event->len, event->name,
				event_set[test_num].name,
				strncmp(event_set[test_num].name, event->name, event->len));
		}
		test_num++;
		i += EVENT_SIZE + event->len;
	}
	for (; test_num < test_cnt; test_num++) {
		tst_res(TFAIL, "didn't get event: mask=%04x ",
			event_set[test_num].mask);
	}
}

void main(){
	setup();
	cleanup();
}
