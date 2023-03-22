#include "incl.h"
#define INVALID_CMD 100
#define CMD_DESC(x) .cmd = x, .desc = #x
char nobody_uid[] = "nobody";
struct passwd *ltpuser;

struct tcase {
	int cmd;
	const char *desc;
	int exp_errno;
} tcases[] = {
	{CMD_DESC(INVALID_CMD), EINVAL},
	{CMD_DESC(LINUX_REBOOT_CMD_CAD_ON), EPERM},
};

void run(unsigned int n)
{
	struct tcase *tc = &tcases[n];
	if (n == 0)
		TST_EXP_FAIL(reboot(tc->cmd),
			tc->exp_errno, "%s", tc->desc);
	else {
		ltpuser = getpwnam(nobody_uid);
		seteuid(ltpuser->pw_uid);
		TST_EXP_FAIL(reboot(tc->cmd),
			tc->exp_errno, "%s", tc->desc);
		seteuid(0);
	}
}

void main(){
	setup();
	cleanup();
}
