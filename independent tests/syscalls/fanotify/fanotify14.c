#include "incl.h"
#define _GNU_SOURCE
#ifdef HAVE_SYS_FANOTIFY_H
#define MNTPOINT "mntpoint"
#define FILE1 MNTPOINT"/file1"
#define INODE_EVENTS (FAN_ATTRIB | FAN_CREATE | FAN_DELETE | FAN_MOVE | \
		      FAN_DELETE_SELF | FAN_MOVE_SELF)
#define FLAGS_DESC(flags) {(flags), (#flags)}

int fanotify_fd;

int fan_report_target_fid_unsupported;

int ignore_mark_unsupported;
struct test_case_flags_t {
	unsigned long long flags;
	const char *desc;
};

struct test_case_t {
	struct test_case_flags_t init;
	struct test_case_flags_t mark;
	struct test_case_flags_t mask;
	int expected_errno;
} test_cases[] = {
	{FLAGS_DESC(FAN_CLASS_CONTENT | FAN_REPORT_FID), {}, {}, EINVAL},
	{FLAGS_DESC(FAN_CLASS_PRE_CONTENT | FAN_REPORT_FID), {}, {}, EINVAL},
	{FLAGS_DESC(FAN_CLASS_NOTIF), FLAGS_DESC(0), FLAGS_DESC(INODE_EVENTS),
		EINVAL},
	{FLAGS_DESC(FAN_CLASS_NOTIF | FAN_REPORT_FID),
		FLAGS_DESC(FAN_MARK_MOUNT), FLAGS_DESC(INODE_EVENTS), EINVAL},
	{FLAGS_DESC(FAN_CLASS_NOTIF | FAN_REPORT_NAME), {}, {}, EINVAL},
	{FLAGS_DESC(FAN_CLASS_NOTIF | FAN_REPORT_FID | FAN_REPORT_NAME), {},
		{}, EINVAL},
	{FLAGS_DESC(FAN_CLASS_NOTIF | FAN_REPORT_TARGET_FID | FAN_REPORT_DFID_NAME),
		{}, {}, EINVAL},
	{FLAGS_DESC(FAN_CLASS_NOTIF | FAN_REPORT_TARGET_FID | FAN_REPORT_DFID_FID),
		{}, {}, EINVAL},
	{FLAGS_DESC(FAN_CLASS_NOTIF | FAN_REPORT_DFID_FID), FLAGS_DESC(0),
		FLAGS_DESC(FAN_RENAME), EINVAL},
	{FLAGS_DESC(FAN_CLASS_NOTIF), FLAGS_DESC(FAN_MARK_ONLYDIR),
		FLAGS_DESC(FAN_OPEN), ENOTDIR},
	{FLAGS_DESC(FAN_CLASS_NOTIF | FAN_REPORT_DFID_NAME_TARGET),
		FLAGS_DESC(0), FLAGS_DESC(FAN_DELETE), ENOTDIR},
	{FLAGS_DESC(FAN_CLASS_NOTIF | FAN_REPORT_DFID_NAME_TARGET),
		FLAGS_DESC(0), FLAGS_DESC(FAN_RENAME), ENOTDIR},
	{FLAGS_DESC(FAN_CLASS_NOTIF | FAN_REPORT_DFID_NAME_TARGET),
		FLAGS_DESC(0), FLAGS_DESC(FAN_OPEN | FAN_ONDIR), ENOTDIR},
	{FLAGS_DESC(FAN_CLASS_NOTIF | FAN_REPORT_DFID_NAME_TARGET),
		FLAGS_DESC(0), FLAGS_DESC(FAN_OPEN | FAN_EVENT_ON_CHILD),
		ENOTDIR},
	{FLAGS_DESC(FAN_CLASS_NOTIF | FAN_REPORT_DFID_NAME),
		FLAGS_DESC(FAN_MARK_IGNORE_SURV), FLAGS_DESC(FAN_DELETE),
		ENOTDIR},
	{FLAGS_DESC(FAN_CLASS_NOTIF | FAN_REPORT_DFID_NAME),
		FLAGS_DESC(FAN_MARK_IGNORE_SURV), FLAGS_DESC(FAN_RENAME),
		ENOTDIR},
	{FLAGS_DESC(FAN_CLASS_NOTIF | FAN_REPORT_DFID_NAME),
		FLAGS_DESC(FAN_MARK_IGNORE_SURV),
		FLAGS_DESC(FAN_OPEN | FAN_ONDIR), ENOTDIR},
	{FLAGS_DESC(FAN_CLASS_NOTIF | FAN_REPORT_DFID_NAME),
		FLAGS_DESC(FAN_MARK_IGNORE_SURV),
		FLAGS_DESC(FAN_OPEN | FAN_EVENT_ON_CHILD), ENOTDIR},
	{FLAGS_DESC(FAN_CLASS_NOTIF), FLAGS_DESC(FAN_MARK_IGNORE),
		FLAGS_DESC(FAN_OPEN), EISDIR},
	{FLAGS_DESC(FAN_CLASS_NOTIF),
		FLAGS_DESC(FAN_MARK_MOUNT | FAN_MARK_IGNORE),
		FLAGS_DESC(FAN_OPEN), EINVAL},
	{FLAGS_DESC(FAN_CLASS_NOTIF),
		FLAGS_DESC(FAN_MARK_FILESYSTEM | FAN_MARK_IGNORE),
		FLAGS_DESC(FAN_OPEN), EINVAL},
};

void do_test(unsigned int number)
{
	struct test_case_t *tc = &test_cases[number];
	tst_res(TINFO, "Test case %d: fanotify_init(%s, O_RDONLY)", number,
		tc->init.desc);
	if (fan_report_target_fid_unsupported && tc->init.flags & FAN_REPORT_TARGET_FID) {
		FANOTIFY_INIT_FLAGS_ERR_MSG(FAN_REPORT_TARGET_FID,
					    fan_report_target_fid_unsupported);
		return;
	}
	if (ignore_mark_unsupported && tc->mark.flags & FAN_MARK_IGNORE) {
		tst_res(TCONF, "FAN_MARK_IGNORE not supported in kernel?");
		return;
	}
	if (!tc->mask.flags && tc->expected_errno) {
		TST_EXP_FAIL(fanotify_init(tc->init.flags, O_RDONLY),
			tc->expected_errno);
	} else {
		TST_EXP_FD(fanotify_init(tc->init.flags, O_RDONLY));
	}
	fanotify_fd = TST_RET;
	if (fanotify_fd < 0)
		return;
	if (!tc->mask.flags)
		goto out;
	const char *path = tc->expected_errno == ENOTDIR ? FILE1 : MNTPOINT;
	tst_res(TINFO, "Testing fanotify_mark(FAN_MARK_ADD | %s, %s)",
		tc->mark.desc, tc->mask.desc);
	TST_EXP_FD_OR_FAIL(fanotify_mark(fanotify_fd, FAN_MARK_ADD | tc->mark.flags,
					 tc->mask.flags, AT_FDCWD, path),
					 tc->expected_errno);
	 * ENOTDIR are errors for events/flags not allowed on a non-dir inode.
	 * Try to set an inode mark on a directory and it should succeed.
	 * Try to set directory events in filesystem mark mask on non-dir
	 * and it should succeed.
	 */
	if (TST_PASS && tc->expected_errno == ENOTDIR) {
		fanotify_mark(fanotify_fd, FAN_MARK_ADD | tc->mark.flags,
				   tc->mask.flags, AT_FDCWD, MNTPOINT);
		tst_res(TPASS,
			"Adding an inode mark on directory did not fail with "
			"ENOTDIR error as on non-dir inode");
		if (!(tc->mark.flags & FAN_MARK_ONLYDIR)) {
			fanotify_mark(fanotify_fd, FAN_MARK_ADD | tc->mark.flags |
					   FAN_MARK_FILESYSTEM, tc->mask.flags,
					   AT_FDCWD, FILE1);
			tst_res(TPASS,
				"Adding a filesystem mark on non-dir did not fail with "
				"ENOTDIR error as with an inode mark");
		}
	}
out:
	if (fanotify_fd > 0)
		close(fanotify_fd);
}

void do_setup(void)
{
	REQUIRE_FANOTIFY_INIT_FLAGS_SUPPORTED_ON_FS(FAN_REPORT_FID, MNTPOINT);
	fan_report_target_fid_unsupported =
		fanotify_init_flags_supported_on_fs(FAN_REPORT_DFID_NAME_TARGET, MNTPOINT);
	ignore_mark_unsupported = fanotify_mark_supported_by_kernel(FAN_MARK_IGNORE_SURV);
	fprintf(FILE1, "0");
}

void do_cleanup(void)
{
	if (fanotify_fd > 0)
		close(fanotify_fd);
}

void main(){
	setup();
	cleanup();
}
