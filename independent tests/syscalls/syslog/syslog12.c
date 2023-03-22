#include "incl.h"
#define syslog(arg1, arg2, arg3) tst_syscall(__NR_syslog, arg1, arg2, arg3)

char buf;

struct passwd *ltpuser;

void setup(void)
{
	ltpuser = getpwnam("nobody");
}

void setup_nonroot(void)
{
	setegid(ltpuser->pw_gid);
	seteuid(ltpuser->pw_uid);
}

void cleanup_nonroot(void)
{
	seteuid(0);
}

struct tcase {
	int type;
	char *buf;
	int len;
	int exp_errno;
	char *desc;
} tcases[] = {
	{100, &buf, 0, EINVAL, "invalid type/command"},
	{2, NULL, 0, EINVAL, "NULL buffer argument"},
	{3, &buf, -1, EINVAL, "negative length argument"},
	{8, &buf, -1, EINVAL, "console level less than 0"},
	{8, &buf, 9, EINVAL, "console level greater than 8"},
	{2, &buf, 0, EPERM, "non-root user"},
};

void run(unsigned int n)
{
	struct tcase *tc = &tcases[n];
	if (n == ARRAY_SIZE(tcases)-1)
		setup_nonroot();
	TST_EXP_FAIL(syslog(tc->type, tc->buf, tc->len), tc->exp_errno,
		"syslog() with %s", tc->desc);
	if (n == ARRAY_SIZE(tcases)-1)
		cleanup_nonroot();
}

void main(){
	setup();
	cleanup();
}
