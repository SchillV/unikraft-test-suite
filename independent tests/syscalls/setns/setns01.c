#include "incl.h"
#define _GNU_SOURCE

const char nobody_uid[] = "nobody";

struct passwd *ltpuser;

int regular_fd;
struct testcase_t {
	const char *msg;
	int fd;
	int ns_type;
	int exp_ret;
	int exp_errno;
	int skip;
	void (*setup) (struct testcase_t *, int i);
	void (*cleanup) (struct testcase_t *);
};

void setup0(struct testcase_t *t, int i)
{
	t->ns_type = ns_types[i];
}

void setup1(struct testcase_t *t, int i)
{
	t->ns_type = ns_types[i];
	t->fd = regular_fd;
}

void setup2(struct testcase_t *t, int i)
{
	t->fd = ns_fds[i];
}

void setup3(struct testcase_t *t, int i)
{
	if (ns_total < 2) {
		t->skip = 1;
		return;
	}
	t->fd = ns_fds[i];
	t->ns_type = ns_types[(i+1) % ns_total];
}

void setup4(struct testcase_t *t, int i)
{
	seteuid(ltpuser->pw_uid);
	t->fd = ns_fds[i];
	t->ns_type = ns_types[i];
}

void cleanup4(LTP_ATTRIBUTE_UNUSED struct testcase_t *t)
{
	seteuid(0);
}

struct testcase_t tcases[] = {
	{
		.msg = "invalid fd",
		.fd = -1,
		.exp_ret = -1,
		.exp_errno = EBADF,
		.setup = setup0,
	},
	{
		.msg = "regular file fd",
		.exp_ret = -1,
		.exp_errno = EINVAL,
		.setup = setup1,
	},
	{
		.msg = "invalid ns_type",
		.ns_type = -1,
		.exp_ret = -1,
		.exp_errno = EINVAL,
		.setup = setup2,
	},
	{
		.msg = "mismatch ns_type/fd",
		.exp_ret = -1,
		.exp_errno = EINVAL,
		.setup = setup3,
	},
	{
		.msg = "without CAP_SYS_ADMIN",
		.exp_ret = -1,
		.exp_errno = EPERM,
		.setup = setup4,
		.cleanup = cleanup4,
	}
};

void test_setns(unsigned int testno)
{
	int ret, i;
	struct testcase_t *t = &tcases[testno];
	for (i = 0; i < ns_total; i++) {
		if (t->setup)
			t->setup(t, i);
		if (t->skip) {
			tst_res(TCONF, "skip %s", t->msg);
			continue;
		}
		tst_res(TINFO, "setns(%d, 0x%x)", t->fd, t->ns_type);
		ret = tst_syscall(__NR_setns, t->fd, t->ns_type);
		if (ret == t->exp_ret) {
			if (ret == -1 && errno == t->exp_errno) {
				tst_res(TPASS, "%s exp_errno=%d (%s)",
					t->msg,	t->exp_errno,
					tst_strerrno(t->exp_errno));
			} else {
				tst_res(TFAIL|TERRNO, "%s exp_errno=%d (%s)",
					t->msg, t->exp_errno,
					tst_strerrno(t->exp_errno));
			}
		} else {
			tst_res(TFAIL, "%s ret=%d expected=%d",
				t->msg,	ret, t->exp_ret);
		}
		if (t->cleanup)
			t->cleanup(t);
	}
}

void setup(void)
{
	tst_syscall(__NR_setns, -1, 0);
	init_available_ns();
	if (ns_total == 0)
		tst_brk(TCONF, "no ns types/proc entries");
	ltpuser = getpwnam(nobody_uid);
	regular_fd = open("dummy", O_RDWR|O_CREAT, 0600);
	unlink("dummy");
}

void cleanup(void)
{
	close_ns_fds();
	if (regular_fd)
		close(regular_fd);
}

void main(){
	setup();
	cleanup();
}
