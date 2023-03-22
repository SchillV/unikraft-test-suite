#include "incl.h"

int exp_eno;

int  verify_sysctl(
{
	char *osname = "Linux";
	int name[] = {CTL_KERN, KERN_OSTYPE};
	struct __sysctl_args args = {
		.name = name,
		.nlen = ARRAY_SIZE(name),
		.newval = osname,
		.newlen = sizeof(osname),
	};
tst_syscall(__NR__sysctl, &args);
	if (TST_RET != -1) {
		tst_res(TFAIL, "sysctl(2) succeeded unexpectedly");
		return;
	}
	if (TST_ERR == exp_eno) {
		tst_res(TPASS | TTERRNO, "Got expected error");
	} else {
		tst_res(TFAIL | TTERRNO, "Got unexpected error, expected %s",
			tst_strerrno(exp_eno));
	}
}

void setup(void)
{
	tst_res(TINFO,
		 "this test's results are based on potentially undocumented behavior in the kernel. read the NOTE in the source file for more details");
	exp_eno = EACCES;
}

void do_test(void)
{
	pid_t pid;
	struct passwd *ltpuser;
	pid = fork();
	if (!pid) {
		ltpuser = getpwnam("nobody");
		setuid(ltpuser->pw_uid);
int 		verify_sysctl();
	} else {
int 		verify_sysctl();
		tst_reap_children();
	}
}

