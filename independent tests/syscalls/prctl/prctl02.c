#include "incl.h"
#define OPTION_INVALID 999
#define unsup_string "prctl() doesn't support this option, skip it"

const struct sock_filter  strict_filter[] = {
	BPF_STMT(BPF_RET | BPF_K, SECCOMP_RET_ALLOW)
};

const struct sock_fprog strict = {
	.len = (unsigned short)(sizeof(strict_filter)/sizeof(strict_filter[0])),
	.filter = (struct sock_filter *)strict_filter
};

unsigned long strict_addr = (unsigned long)&strict;

unsigned long bad_addr;

unsigned long num_0;

unsigned long num_1 = 1;

unsigned long num_2 = 2;

unsigned long num_PR_CAP_AMBIENT_CLEAR_ALL = PR_CAP_AMBIENT_CLEAR_ALL;

unsigned long num_PR_CAP_AMBIENT_IS_SET = PR_CAP_AMBIENT_IS_SET;

unsigned long num_invalid = ULONG_MAX;

int seccomp_nsup;

int nonewprivs_nsup;

int thpdisable_nsup;

int capambient_nsup;

int speculationctrl_nsup;

struct tcase {
	int option;
	unsigned long *arg2;
	unsigned long *arg3;
	int exp_errno;
	char *tname;
} tcases[] = {
	{OPTION_INVALID, &num_1, &num_0, EINVAL, "invalid option"},
	{PR_SET_PDEATHSIG, &num_invalid, &num_0, EINVAL, "PR_SET_PDEATHSIG"},
	{PR_SET_DUMPABLE, &num_2, &num_0, EINVAL, "PR_SET_DUMPABLE"},
	{PR_SET_NAME, &bad_addr, &num_0, EFAULT, "PR_SET_NAME"},
	{PR_SET_SECCOMP, &num_2, &bad_addr, EFAULT, "PR_SET_SECCOMP"},
	{PR_SET_SECCOMP, &num_2, &strict_addr, EACCES, "PR_SET_SECCOMP"},
	{PR_SET_TIMING, &num_1, &num_0, EINVAL, "PR_SET_TIMING"},
	{PR_SET_NO_NEW_PRIVS, &num_0, &num_0, EINVAL, "PR_SET_NO_NEW_PRIVS"},
	{PR_SET_NO_NEW_PRIVS, &num_1, &num_1, EINVAL, "PR_SET_NO_NEW_PRIVS"},
	{PR_GET_NO_NEW_PRIVS, &num_1, &num_0, EINVAL, "PR_GET_NO_NEW_PRIVS"},
	{PR_SET_THP_DISABLE, &num_0, &num_1, EINVAL, "PR_SET_THP_DISABLE"},
	{PR_GET_THP_DISABLE, &num_1, &num_1, EINVAL, "PR_GET_THP_DISABLE"},
	{PR_CAP_AMBIENT, &num_invalid, &num_0, EINVAL, "PR_CAP_AMBIENT"},
	{PR_CAP_AMBIENT, &num_PR_CAP_AMBIENT_CLEAR_ALL, &num_1, EINVAL, "PR_CAP_AMBIENT"},
	{PR_CAP_AMBIENT, &num_PR_CAP_AMBIENT_IS_SET, &num_invalid, EINVAL, "PR_CAP_AMBIENT"},
	{PR_GET_SPECULATION_CTRL, &num_0, &num_invalid, EINVAL, "PR_GET_SPECULATION_CTRL"},
	{PR_SET_SECUREBITS, &num_0, &num_0, EPERM, "PR_SET_SECUREBITS"},
	{PR_CAPBSET_DROP, &num_1, &num_0, EPERM, "PR_CAPBSET_DROP"},
};

int verify_prctl(unsigned int n)
{
	struct tcase *tc = &tcases[n];
	printf("[I] Test #%d: %s\n", n, tc->tname);
	switch (tc->option) {
	case PR_SET_SECCOMP:
		if (seccomp_nsup) {
			printf("%s\n", unsup_string);
			return 0;
		}
	break;
	case PR_GET_NO_NEW_PRIVS:
	case PR_SET_NO_NEW_PRIVS:
		if (nonewprivs_nsup) {
			printf("%s\n", unsup_string);
			return 0;
		}
	break;
	case PR_SET_THP_DISABLE:
	case PR_GET_THP_DISABLE:
		if (thpdisable_nsup) {
			printf("%s\n", unsup_string);
			return 0;
		}
	break;
	case PR_CAP_AMBIENT:
		if (capambient_nsup) {
			printf("%s\n", unsup_string);
			return 0;
		}
	break;
	case PR_GET_SPECULATION_CTRL:
		if (speculationctrl_nsup) {
			printf("%s\n", unsup_string);
			return 0;
		}
	break;
	default:
	break;
	}
	int ret = prctl(tc->option, *tc->arg2, *tc->arg3, 0, 0);
	if (ret == 0) {
		printf("prctl() succeeded unexpectedly\n");
		return 0;
	}
	if (tc->exp_errno == errno) {
		printf("prctl() failed as expected\n");
		return 1;
	} else {
		if (tc->option == PR_SET_SECCOMP && errno == EINVAL)
			printf("current system was not built with CONFIG_SECCOMP_FILTER.\n");
		else
			printf("prctl() failed unexpectedly, expected %d, got %d\n",tc->exp_errno, errno);
	}
}

void setup(void)
{
	bad_addr = (unsigned long)NULL;
	int ret = prctl(PR_GET_SECCOMP);
	if (errno == EINVAL)
		seccomp_nsup = 1;
	ret = prctl(PR_GET_NO_NEW_PRIVS, 0, 0, 0, 0);
	if (errno == EINVAL)
		nonewprivs_nsup = 1;
	ret = prctl(PR_GET_THP_DISABLE, 0, 0, 0, 0);
	if (errno == EINVAL)
		thpdisable_nsup = 1;
	ret = prctl(PR_CAP_AMBIENT, PR_CAP_AMBIENT_CLEAR_ALL, 0, 0, 0);
	if (errno == EINVAL)
		capambient_nsup = 1;
	ret = prctl(PR_GET_SPECULATION_CTRL, 0, 0, 0, 0);
	if (errno == EINVAL)
		speculationctrl_nsup = 1;
}

void main(){
	setup();
	int ok = 1;
	for(int i=0;i<18;i++)
		if(verify_prctl(i) == 0)
			ok=0;
	if(ok)
		printf("test succeeded\n");
}
