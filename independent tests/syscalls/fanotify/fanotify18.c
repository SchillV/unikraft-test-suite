#include "incl.h"
#define _GNU_SOURCE
#ifdef HAVE_SYS_FANOTIFY_H
#define DISALLOWED_INIT_FLAGS	(FAN_UNLIMITED_QUEUE | FAN_UNLIMITED_MARKS | \
				 FAN_CLASS_CONTENT | FAN_CLASS_PRE_CONTENT | \
				 FAN_REPORT_TID)
#define DISALLOWED_MARK_FLAGS	(FAN_MARK_MOUNT | FAN_MARK_FILESYSTEM)
#define MOUNT_PATH	"fs_mnt"
#define TEST_FILE	MOUNT_PATH "/testfile"

int fd_notify;

struct test_case_t {
	const char *name;
	unsigned long init_flags;
	unsigned long mark_flags;
	unsigned long long mark_mask;
} test_cases[] = {
	{
		"init_flags: missing FAN_REPORT_FID",
		FAN_CLASS_NOTIF,
		0, 0
	},
	{
		"init_flags: FAN_CLASS_CONTENT",
		FANOTIFY_REQUIRED_USER_INIT_FLAGS | FAN_CLASS_CONTENT,
		0, 0
	},
	{
		"init_flags: FAN_CLASS_PRE_CONTENT",
		FANOTIFY_REQUIRED_USER_INIT_FLAGS | FAN_CLASS_PRE_CONTENT,
		0, 0
	},
	{
		"init_flags: FAN_UNLIMITED_QUEUE",
		FANOTIFY_REQUIRED_USER_INIT_FLAGS | FAN_UNLIMITED_QUEUE,
		0, 0
	},
	{
		"init_flags: FAN_UNLIMITED_MARKS",
		FANOTIFY_REQUIRED_USER_INIT_FLAGS | FAN_UNLIMITED_MARKS,
		0, 0
	},
	{
		"init_flags: FAN_REPORT_TID",
		FANOTIFY_REQUIRED_USER_INIT_FLAGS | FAN_REPORT_TID,
		0, 0
	},
	{
		"init_flags: FAN_CLASS_NOTIF, "
		"mark_flags: FAN_MARK_ADD | FAN_MARK_MOUNT",
		FANOTIFY_REQUIRED_USER_INIT_FLAGS | FAN_CLASS_NOTIF,
		FAN_MARK_ADD | FAN_MARK_MOUNT, FAN_ALL_EVENTS
	},
	{
		"init_flags: FAN_CLASS_NOTIF, "
		"mark_flags: FAN_MARK_ADD | FAN_MARK_FILESYSTEM",
		FANOTIFY_REQUIRED_USER_INIT_FLAGS | FAN_CLASS_NOTIF,
		FAN_MARK_ADD | FAN_MARK_FILESYSTEM, FAN_ALL_EVENTS
	},
	{
		"init_flags: FAN_CLASS_NOTIF, "
		"mark_flags: FAN_MARK_ADD, "
		"mark_mask: FAN_ALL_EVENTS",
		FANOTIFY_REQUIRED_USER_INIT_FLAGS | FAN_CLASS_NOTIF,
		FAN_MARK_ADD, FAN_ALL_EVENTS
	}
};

void test_fanotify(unsigned int n)
{
	struct test_case_t *tc = &test_cases[n];
	tst_res(TINFO, "Test #%d %s", n, tc->name);
	fd_notify = fanotify_init(tc->init_flags, O_RDONLY);
	if (fd_notify < 0) {
		if (errno == EPERM &&
		    ((tc->init_flags & DISALLOWED_INIT_FLAGS) ||
		     (tc->init_flags & FANOTIFY_REQUIRED_USER_INIT_FLAGS) !=
		      FANOTIFY_REQUIRED_USER_INIT_FLAGS)) {
			tst_res(TPASS, "Received result EPERM, as expected");
			return;
		}
		tst_brk(TBROK | TERRNO,
			"fanotify_init(0x%lx, O_RDONLY) failed",
			tc->init_flags);
	}
	if (fanotify_mark(fd_notify, tc->mark_flags, tc->mark_mask, AT_FDCWD,
				TEST_FILE) < 0) {
		 * Unprivileged users are only allowed to mark inodes and not
		 * permitted to use access permissions
		 */
		if (errno == EPERM &&
			(tc->mark_flags & DISALLOWED_MARK_FLAGS ||
			 tc->mark_mask & FAN_ALL_PERM_EVENTS)) {
			tst_res(TPASS, "Received result EPERM, as expected");
			goto out;
		}
		tst_brk(TBROK | TERRNO,
			"fanotify_mark(%d, %lx, %llx, AT_FDCWD, %s) "
			"failed",
			fd_notify,
			tc->mark_flags,
			tc->mark_mask,
			TEST_FILE);
	}
	tst_res(TPASS,
		"fanotify_init() and fanotify_mark() returned successfully, "
		"as expected");
out:
	close(fd_notify);
}

void setup(void)
{
	int fd;
	struct passwd *nobody;
	touch(TEST_FILE, 0666, NULL);
	REQUIRE_FANOTIFY_INIT_FLAGS_SUPPORTED_ON_FS(FAN_REPORT_FID, TEST_FILE);
	if (geteuid() == 0) {
		tst_res(TINFO,
			"Running as privileged user, revoking permissions.");
		nobody = getpwnam("nobody");
		setuid(nobody->pw_uid);
	}
	fd = fanotify_init(FANOTIFY_REQUIRED_USER_INIT_FLAGS, O_RDONLY);
	if (fd < 0) {
		tst_brk(TCONF,
			"unprivileged fanotify not supported by kernel?");
	}
	close(fd);
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
