#include "incl.h"
#define _GNU_SOURCE

const char *username = "ltp_setpriority01";

int pid, uid, user_added;

struct tcase {
	int which;
	int *who;
} tcases[] = {
	{PRIO_PROCESS, &pid},
	{PRIO_PGRP, &pid},
	{PRIO_USER, &uid}
};

const char *str_which(int which)
{
	switch (which) {
	case PRIO_PROCESS:
		return "PRIO_PROCESS";
	case PRIO_PGRP:
		return "PRIO_PGRP";
	case PRIO_USER:
		return "PRIO_USER";
	default:
		return "???";
	}
}

void setpriority_test(struct tcase *tc)
{
	int new_prio, cur_prio;
	int failflag = 0;
	for (new_prio = -20; new_prio < 20; new_prio++) {
setpriority(tc->which, *tc->who, new_prio);
		if (TST_RET != 0) {
			tst_res(TFAIL | TTERRNO,
				"setpriority(%d, %d, %d) failed",
				tc->which, *tc->who, new_prio);
			failflag = 1;
			continue;
		}
		cur_prio = getpriority(tc->which, *tc->who);
		if (cur_prio != new_prio) {
			tst_res(TFAIL, "current priority(%d) and "
				"new priority(%d) do not match",
				cur_prio, new_prio);
			failflag = 1;
		}
	}
	if (!failflag) {
		tst_res(TPASS, "setpriority(%s(%d), %d, -20..19) succeeded",
			str_which(tc->which), tc->which, *tc->who);
	}
}

int  verify_setpriority(unsigned int n)
{
	struct tcase *tc = &tcases[n];
	if (tc->which == PRIO_USER && !user_added) {
		tst_res(TCONF, "setpriority(%s(%d), %d, -20..19) skipped - Can't add user",
			str_which(tc->which), tc->which, *tc->who);
		return;
	}
	pid = fork();
	if (pid == 0) {
		if (user_added)
			setuid(uid);
		setpgid(0, 0);
		TST_CHECKPOINT_WAKE_AND_WAIT(0);
		exit(0);
	}
	TST_CHECKPOINT_WAIT(0);
	setpriority_test(tc);
	TST_CHECKPOINT_WAKE(0);
	tst_reap_children();
}

void setup(void)
{
	const char *const cmd_useradd[] = {"useradd", username, NULL};
	struct passwd *ltpuser;
	int rc;
	switch ((rc = tst_cmd(cmd_useradd, NULL, NULL, TST_CMD_PASS_RETVAL))) {
	case 0:
		user_added = 1;
		ltpuser = getpwnam(username);
		uid = ltpuser->pw_uid;
		return;
	case 1:
	case 255:
		return;
	default:
		tst_brk(TBROK, "Useradd failed (%d)", rc);
	}
}

void cleanup(void)
{
	if (!user_added)
		return;
	const char *const cmd_userdel[] = {"userdel", "-r", username, NULL};
	if (tst_cmd(cmd_userdel, NULL, NULL, TST_CMD_PASS_RETVAL))
		tst_res(TWARN | TERRNO, "'userdel -r %s' failed", username);
}

void main(){
	setup();
	cleanup();
}
