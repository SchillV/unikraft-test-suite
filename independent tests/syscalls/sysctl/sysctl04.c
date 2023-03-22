#include "incl.h"

char osname[BUFSIZ];

size_t length = BUFSIZ;

struct tcase {
	int name[2];
	int nlen;
	void *oldval;
	size_t *oldlen;
	int exp_err;
} tcases[] = {
	{{CTL_KERN, KERN_OSREV}, 0, osname, &length, ENOTDIR},
	{{CTL_KERN, KERN_OSREV}, CTL_MAXNAME + 1, osname, &length, ENOTDIR},
	{{CTL_KERN, KERN_OSRELEASE}, 2, (void *) -1, &length, EFAULT},
	{{CTL_KERN, KERN_VERSION}, 2, osname, (void *) -1, EFAULT},
};

int  verify_sysctl(unsigned int n)
{
	struct tcase *tc = &tcases[n];
	struct __sysctl_args args = {
		.name = tc->name,
		.nlen = tc->nlen,
		.oldval = tc->oldval,
		.oldlenp = tc->oldlen,
	};
tst_syscall(__NR__sysctl, &args);
	if (TST_RET != -1) {
		tst_res(TFAIL, "sysctl(2) succeeded unexpectedly");
		return;
	}
	if (TST_ERR == tc->exp_err) {
		tst_res(TPASS | TTERRNO, "Got expected error");
	} else {
		tst_res(TFAIL | TTERRNO, "Got unexpected error, expected %s",
			tst_strerrno(tc->exp_err));
	}
}

void main(){
	setup();
	cleanup();
}
