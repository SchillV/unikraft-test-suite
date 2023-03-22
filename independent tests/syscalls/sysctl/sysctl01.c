#include "incl.h"

struct utsname buf;

struct tcase {
	char *desc;
	int name[2];
	char *cmp_str;
} tcases[] = {
	{"KERN_OSTYPE", {CTL_KERN, KERN_OSTYPE}, buf.sysname},
	{"KERN_OSRELEASE", {CTL_KERN, KERN_OSRELEASE}, buf.release},
	{"KERN_VERSION", {CTL_KERN, KERN_VERSION}, buf.version},
};

int  verify_sysctl(unsigned int n)
{
	char osname[BUFSIZ];
	size_t length = BUFSIZ;
	struct tcase *tc = &tcases[n];
	memset(osname, 0, BUFSIZ);
	struct __sysctl_args args = {
		.name = tc->name,
		.nlen = ARRAY_SIZE(tc->name),
		.oldval = osname,
		.oldlenp = &length,
	};
tst_syscall(__NR__sysctl, &args);
	if (TST_RET != 0) {
		tst_res(TFAIL | TTERRNO, "sysctl() failed unexpectedly");
		return;
	}
	if (strcmp(osname, tc->cmp_str)) {
		tst_res(TFAIL, "Strings don't match %s : %s",
			osname, tc->cmp_str);
	} else {
		tst_res(TPASS, "Test for %s is correct", tc->desc);
	}
}

void setup(void)
{
	if (uname(&buf) == -1)
		tst_brk(TBROK | TERRNO, "uname() failed");
	char *klp_tag;
	char *right_brace;
	klp_tag = strstr(buf.version, "/kGraft-");
	if (!klp_tag)
		klp_tag = strstr(buf.version, "/lp-");
	if (klp_tag) {
		right_brace = strchr(klp_tag, ')');
		if (right_brace)
			memmove(klp_tag, right_brace, strlen(right_brace)+1);
	}
}

void main(){
	setup();
	cleanup();
}
