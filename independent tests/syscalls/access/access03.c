#include "incl.h"

uid_t uid;

struct tcase {
	void *addr;
	int mode;
	char *name;
} tcases[] = {
	{(void *)-1, F_OK, "F_OK"},
	{(void *)-1, R_OK, "R_OK"},
	{(void *)-1, W_OK, "W_OK"},
	{(void *)-1, X_OK, "X_OK"},
};

int  verify_access(unsigned int n)
{
	struct tcase *tc = &tcases[n];
	pid_t pid;
	TST_EXP_FAIL(access(tc->addr, tc->mode), EFAULT,
	             "invalid address as root");
	pid = fork();
	if (pid) {
		waitpid(pid, NULL, 0);
	} else {
		setuid(uid);
		TST_EXP_FAIL(access(tc->addr, tc->mode), EFAULT,
		             "invalid address as nobody");
	}
}

void setup(void)
{
	struct passwd *pw;
	pw = getpwnam("nobody");
	uid = pw->pw_uid;
}

void main(){
	setup();
	cleanup();
}
