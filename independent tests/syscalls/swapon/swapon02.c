#include "incl.h"

void setup01(void);

void cleanup01(void);

uid_t nobody_uid;

int do_swapoff;

struct tcase {
	char *err_desc;
	int exp_errno;
	char *exp_errval;
	char *path;
	void (*setup)(void);
	void (*cleanup)(void);
} tcases[] = {
	{"Path does not exist", ENOENT, "ENOENT", "./doesnotexist", NULL, NULL},
	{"Invalid path", EINVAL, "EINVAL", "./notswap", NULL, NULL},
	{"Permission denied", EPERM, "EPERM", "./swapfile01", setup01, cleanup01},
	{"File already used", EBUSY, "EBUSY", "./alreadyused", NULL, NULL},
};

void setup01(void)
{
	seteuid(nobody_uid);
}

void cleanup01(void)
{
	seteuid(0);
}

void setup(void)
{
	struct passwd *nobody;
	nobody = getpwnam("nobody");
	nobody_uid = nobody->pw_uid;
	is_swap_supported("./tstswap");
	touch("notswap", 0777, NULL);
	make_swapfile("swapfile01", 0);
	make_swapfile("alreadyused", 0);
	if (tst_syscall(__NR_swapon, "alreadyused", 0))
		tst_res(TWARN | TERRNO, "swapon(alreadyused) failed");
	else
		do_swapoff = 1;
}
void cleanup(void)
{
	if (do_swapoff && tst_syscall(__NR_swapoff, "alreadyused"))
		tst_res(TWARN | TERRNO, "swapoff(alreadyused) failed");
}

int  verify_swapon(unsigned int i)
{
	struct tcase *tc = tcases + i;
	if (tc->setup)
		tc->setup();
tst_syscall(__NR_swapon, tc->path, 0);
	if (tc->cleanup)
		tc->cleanup();
	if (TST_RET == -1 && TST_ERR == tc->exp_errno) {
		tst_res(TPASS, "swapon(2) expected failure;"
			 " Got errno - %s : %s",
			 tc->exp_errval, tc->err_desc);
		return;
	}
	tst_res(TFAIL, "swapon(2) failed to produce expected error:"
	         " %d, errno: %s and got %d.", tc->exp_errno,
	         tc->exp_errval, TST_ERR);
}

void main(){
	setup();
	cleanup();
}
