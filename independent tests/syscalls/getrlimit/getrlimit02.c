#include "incl.h"
#define INVALID_RES_TYPE 1000

struct rlimit rlim;

struct tcase {
	int exp_errno;
	char *desc;
	struct rlimit *rlim;
	int res_type;
} tcases[] = {
	{EFAULT, "invalid address", (void *)-1, RLIMIT_CORE},
	{EINVAL, "invalid resource type", &rlim, INVALID_RES_TYPE}
};

int  verify_getrlimit(unsigned int i)
{
	struct tcase *tc = &tcases[i];
	TST_EXP_FAIL(getrlimit(tc->res_type, tc->rlim),
				tc->exp_errno,
				"getrlimit() with %s",
				tc->desc);
}

void main(){
	setup();
	cleanup();
}
