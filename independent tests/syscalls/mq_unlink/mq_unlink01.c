#include "incl.h"
#define QUEUE_NAME	"/test_mqueue"

uid_t euid;

struct passwd *pw;
struct test_case {
	int as_nobody;
	char *qname;
	int ret;
	int err;
};

struct test_case tcase[] = {
	{
		.qname = QUEUE_NAME,
		.ret = 0,
		.err = 0,
	},
	{
		.as_nobody = 1,
		.qname = QUEUE_NAME,
		.ret = -1,
		.err = EACCES,
	},
	{
		.qname = "/aaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaa"
			"aaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaa"
			"aaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaa"
			"aaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaa"
			"aaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaa"
			"aaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaa"
			"aaaaaaaaaaaaaaa",
		.ret = -1,
		.err = ENOENT,
	},
	{
		.qname = "/aaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaa"
			"aaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaa"
			"aaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaa"
			"aaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaa"
			"aaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaa"
			"aaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaa"
			"aaaaaaaaaaaaaaaa",
		.ret = -1,
		.err = ENAMETOOLONG,
	},
};
void setup(void)
{
	euid = geteuid();
	pw = getpwnam("nobody");
}

void do_test(unsigned int i)
{
	struct test_case *tc = &tcase[i];
	mqd_t fd;
	tst_res(TINFO, "queue name %s", tc->qname);
	 * When test ended with SIGTERM etc, mq descriptor is left remains.
	 * So we delete it first.
	 */
	mq_unlink(QUEUE_NAME);
	fd = mq_open(QUEUE_NAME, O_CREAT | O_EXCL | O_RDWR, S_IRWXU, NULL);
	if (tc->as_nobody && seteuid(pw->pw_uid)) {
		tst_res(TFAIL | TERRNO, "seteuid failed");
		goto EXIT;
	}
mq_unlink(tc->qname);
	if (TST_ERR != tc->err || TST_RET != tc->ret) {
		tst_res(TFAIL | TTERRNO, "mq_unlink returned %ld, expected %d,"
			" expected errno %s (%d)", TST_RET,
			tc->ret, tst_strerrno(tc->err), tc->err);
	} else {
		tst_res(TPASS | TTERRNO, "mq_unlink returned %ld", TST_RET);
	}
EXIT:
	if (tc->as_nobody && seteuid(euid) == -1)
		tst_res(TWARN | TERRNO, "seteuid back to %d failed", euid);
	if (fd > 0 && close(fd))
		tst_res(TWARN | TERRNO, "close(fd) failed");
	mq_unlink(QUEUE_NAME);
}

