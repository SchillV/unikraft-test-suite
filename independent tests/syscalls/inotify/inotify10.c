#include "incl.h"
#if defined(HAVE_SYS_INOTIFY_H)
# include <sys/inotify.h>
#endif
#if defined(HAVE_SYS_INOTIFY_H)
#define EVENT_MAX 10
#define EVENT_SIZE  (sizeof(struct inotify_event))
#define EVENT_BUF_LEN        (EVENT_MAX * (EVENT_SIZE + 16))
#define BUF_SIZE 256
struct event_t {
	char name[BUF_SIZE];
	unsigned int mask;
	int wd;
};
#define	TEST_DIR	"test_dir"
#define	TEST_FILE	"test_file"

struct tcase {
	const char *tname;
	unsigned int parent_mask;
	unsigned int subdir_mask;
	unsigned int child_mask;
	unsigned int parent_mask_other;
	unsigned int subdir_mask_other;
	unsigned int child_mask_other;
} tcases[] = {
	{
		"Group with parent and child watches",
		IN_ATTRIB, IN_ATTRIB, IN_ATTRIB,
		0, 0, 0,
	},
	{
		"Group with child watches and other group with parent watch",
		0, IN_ATTRIB, IN_ATTRIB,
		IN_ATTRIB, 0, 0,
	},
	{
		"Group with parent watch and other group with child watches",
		IN_ATTRIB, 0, 0,
		0, IN_ATTRIB, IN_ATTRIB,
	},
	{
		"Two Groups with parent and child watches for different events",
		IN_ATTRIB, IN_OPEN, IN_OPEN,
		IN_OPEN, IN_ATTRIB, IN_ATTRIB,
	},
};
struct event_t event_set[EVENT_MAX];
char event_buf[EVENT_BUF_LEN];
int fd_notify, fd_notify_other;

int  verify_inotify(unsigned int n)
{
	struct tcase *tc = &tcases[n];
	int i = 0, test_num = 0, len;
	int wd_parent = 0, wd_subdir = 0, wd_child = 0;
	int test_cnt = 0;
	tst_res(TINFO, "Test #%d: %s", n, tc->tname);
	fd_notify = myinotify_init();
	fd_notify_other = myinotify_init();
	if (tc->parent_mask)
		wd_parent = myinotify_add_watch(fd_notify, ".", tc->parent_mask);
	if (tc->subdir_mask)
		wd_subdir = myinotify_add_watch(fd_notify, TEST_DIR, tc->subdir_mask);
	if (tc->child_mask)
		wd_child = myinotify_add_watch(fd_notify, TEST_FILE, tc->child_mask);
int 	 * Setup watches on "other" group to verify no intereferecne with our group.
	 * We do not check events reported to the "other" group.
	 */
	if (tc->parent_mask_other)
		myinotify_add_watch(fd_notify_other, ".", tc->parent_mask_other);
	if (tc->subdir_mask_other)
		myinotify_add_watch(fd_notify_other, TEST_DIR, tc->subdir_mask_other);
	if (tc->child_mask_other)
		myinotify_add_watch(fd_notify_other, TEST_FILE, tc->child_mask_other);
	 * Generate IN_ATTRIB events on file and subdir that should be reported to parent
	 * dir with name and to children without name if they have IN_ATTRIB in their mask.
	 */
	chmod(TEST_DIR, 0755);
	chmod(TEST_FILE, 0644);
	if (wd_parent && (tc->parent_mask & IN_ATTRIB)) {
		event_set[test_cnt].wd = wd_parent;
		event_set[test_cnt].mask = tc->parent_mask | IN_ISDIR;
		strcpy(event_set[test_cnt].name, TEST_DIR);
		test_cnt++;
	}
	if (wd_subdir && (tc->subdir_mask & IN_ATTRIB)) {
		event_set[test_cnt].wd = wd_subdir;
		event_set[test_cnt].mask = tc->subdir_mask | IN_ISDIR;
		strcpy(event_set[test_cnt].name, "");
		test_cnt++;
	}
	if (wd_parent && (tc->parent_mask & IN_ATTRIB)) {
		event_set[test_cnt].wd = wd_parent;
		event_set[test_cnt].mask = tc->parent_mask;
		strcpy(event_set[test_cnt].name, TEST_FILE);
		test_cnt++;
	}
	if (wd_child && (tc->child_mask & IN_ATTRIB)) {
		event_set[test_cnt].wd = wd_child;
		event_set[test_cnt].mask = tc->child_mask;
		strcpy(event_set[test_cnt].name, "");
		test_cnt++;
	}
	len = read(fd_notify, event_buf, EVENT_BUF_LEN);
	if (len == -1)
		tst_brk(TBROK | TERRNO, "read failed");
	while (i < len) {
		struct event_t *expected = &event_set[test_num];
		struct inotify_event *event;
		event = (struct inotify_event *)&event_buf[i];
		if (test_num >= test_cnt) {
			tst_res(TFAIL,
				"got unnecessary event: "
				"wd=%d mask=%04x len=%u "
				"name=\"%.*s\"", event->wd, event->mask,
				event->len, event->len, event->name);
		} else if (expected->wd == event->wd &&
			   expected->mask == event->mask &&
			   !strncmp(expected->name, event->name, event->len)) {
			tst_res(TPASS,
				"got event: wd=%d mask=%04x "
				"cookie=%u len=%u name=\"%.*s\"",
				event->wd, event->mask, event->cookie,
				event->len, event->len, event->name);
		} else {
			tst_res(TFAIL, "got event: wd=%d (expected %d) "
				"mask=%04x (expected %x) len=%u "
				"name=\"%.*s\" (expected \"%s\")",
				event->wd, expected->wd,
				event->mask, expected->mask,
				event->len, event->len,
				event->name, expected->name);
		}
		test_num++;
		i += EVENT_SIZE + event->len;
	}
	for (; test_num < test_cnt; test_num++) {
		tst_res(TFAIL, "didn't get event: mask=%04x ",
			event_set[test_num].mask);
	}
	close(fd_notify);
	close(fd_notify_other);
}

void setup(void)
{
	mkdir(TEST_DIR, 00700);
	fprintf(TEST_FILE, "1");
}

void cleanup(void)
{
	if (fd_notify > 0)
		close(fd_notify);
	if (fd_notify_other > 0)
		close(fd_notify_other);
}

void main(){
	setup();
	cleanup();
}
