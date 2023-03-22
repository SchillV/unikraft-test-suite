#include "incl.h"
#define _GNU_SOURCE
#ifdef HAVE_UNSHARE

uid_t nobody_uid;

struct test_case_t {
	int mode;
	int expected_error;
	const char *desc;
} tc[] = {
	{-1, EINVAL, "-1"},
	{CLONE_NEWNS, EPERM, "CLONE_NEWNS"}
};

void run(unsigned int i)
{
	pid_t pid = fork();
	if (pid == 0) {
		if (tc[i].expected_error == EPERM)
			setuid(nobody_uid);
		TST_EXP_FAIL(unshare(tc[i].mode), tc[i].expected_error,
			     "unshare(%s)", tc[i].desc);
	}
}

void setup(void)
{
	struct passwd *ltpuser = getpwnam("nobody");
	nobody_uid = ltpuser->pw_uid;
}

void main(){
	setup();
	cleanup();
}
