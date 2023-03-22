#include "incl.h"
#if defined __i386__ || defined(__x86_64__)

struct tcase {
	int level;
	char *desc;
	int exp_errno;
} tcases[] = {
	{4, "Invalid privilege level", EINVAL},
	{1, "Non super-user", EPERM}
};

int  verify_iopl(unsigned int i)
{
iopl(tcases[i].level);
	if ((TST_RET == -1) && (TST_ERR == tcases[i].exp_errno)) {
		tst_res(TPASS | TTERRNO,
			"Expected failure for %s, errno: %d",
			tcases[i].desc, TST_ERR);
	} else {
		tst_res(TFAIL | TTERRNO,
			"%s returned %ld expected -1, expected %s got ",
			tcases[i].desc, TST_RET, tst_strerrno(tcases[i].exp_errno));
	}
}

void setup(void)
{
	struct passwd *pw;
	pw = getpwnam("nobody");
	seteuid(pw->pw_uid);
}

void cleanup(void)
{
	seteuid(0);
}

void main(){
	setup();
	cleanup();
}
