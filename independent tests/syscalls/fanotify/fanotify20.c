#include "incl.h"
#define _GNU_SOURCE
#ifdef HAVE_SYS_FANOTIFY_H
#define MOUNT_PATH	"fs_mnt"
#define FLAGS_DESC(x) .flags = x, .desc = #x

int fd;

struct test_case_t {
	unsigned int flags;
	char *desc;
	int exp_errno;
} test_cases[] = {
	{
		FLAGS_DESC(FAN_REPORT_PIDFD | FAN_REPORT_TID),
		.exp_errno = EINVAL,
	},
	{
		FLAGS_DESC(FAN_REPORT_PIDFD | FAN_REPORT_FID | FAN_REPORT_DFID_NAME),
	},
};

void do_setup(void)
{
	 * An explicit check for FAN_REPORT_PIDFD is performed early on in the
	 * test initialization as it's a prerequisite for all test cases.
	 */
	REQUIRE_FANOTIFY_INIT_FLAGS_SUPPORTED_BY_KERNEL(FAN_REPORT_PIDFD);
}

void do_test(unsigned int i)
{
	struct test_case_t *tc = &test_cases[i];
	tst_res(TINFO, "Test %s on %s", tc->exp_errno ? "fail" : "pass",
		tc->desc);
	TST_EXP_FD_OR_FAIL(fd = fanotify_init(tc->flags, O_RDONLY),
			   tc->exp_errno);
	if (fd > 0)
		close(fd);
}

void do_cleanup(void)
{
	if (fd > 0)
		close(fd);
}

