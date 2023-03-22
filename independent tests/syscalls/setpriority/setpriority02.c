#include "incl.h"
#define NEW_PRIO	-2
#define INVAL_FLAG	-1
#define INVAL_ID	-1
#define INIT_PID	1

uid_t uid;

struct tcase {
	int which;
	int who;
	int prio;
	int exp_errno;
	int unprivil;
} tcases[] = {
	{INVAL_FLAG, 0, NEW_PRIO, EINVAL, 0},
	{PRIO_PROCESS, INVAL_ID, NEW_PRIO, ESRCH, 0},
	{PRIO_PGRP, INVAL_ID, NEW_PRIO, ESRCH, 0},
	{PRIO_USER, INVAL_ID, NEW_PRIO, ESRCH, 0},
	{PRIO_PROCESS, 0, NEW_PRIO, EACCES, 1},
	{PRIO_PGRP, 0, NEW_PRIO, EACCES, 1},
	{PRIO_PROCESS, INIT_PID, NEW_PRIO, EPERM, 1}
};

void setpriority_test(struct tcase *tc)
{
	char *desc = "";
	if (tc->unprivil)
		desc = "as unprivileged user ";
setpriority(tc->which, tc->who, tc->prio);
	if (TST_RET != -1) {
		tst_res(TFAIL,
			"setpriority(%d, %d, %d) %ssucceeds unexpectedly "
			"returned %ld", tc->which, tc->who, tc->prio, desc,
			TST_RET);
		return;
	}
	if (TST_ERR != tc->exp_errno) {
		tst_res(TFAIL | TTERRNO,
			"setpriority(%d, %d, %d) %sshould fail with %s",
			tc->which, tc->who, tc->prio, desc,
			tst_strerrno(tc->exp_errno));
		return;
	}
	tst_res(TPASS | TTERRNO,
		"setpriority(%d, %d, %d) %sfails as expected",
		tc->which, tc->who, tc->prio, desc);
}

int  verify_setpriority(unsigned int n)
{
	struct tcase *tc = &tcases[n];
	if (tc->unprivil) {
		if (!fork()) {
			setuid(uid);
			setpgid(0, 0);
			setpriority_test(tc);
			exit(0);
		}
		tst_reap_children();
	} else {
		setpriority_test(tc);
	}
}

void setup(void)
{
	struct passwd *ltpuser;
	ltpuser = getpwnam("nobody");
	uid = ltpuser->pw_uid;
}

void main(){
	setup();
	cleanup();
}
