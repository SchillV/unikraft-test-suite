#include "incl.h"

char longpathname[PATH_MAX + 2];

struct test_case_t {
	char *name;
	char *desc;
	int exp_errno;
} tcases[] = {
	{"nonexistfile", "non-existent file", ENOENT},
	{"", "path is empty string", ENOENT},
	{"nefile/file", "path contains a non-existent file", ENOENT},
	{NULL, "invalid address", EFAULT},
	{"file/file", "path contains a regular file", ENOTDIR},
	{longpathname, "pathname too long", ENAMETOOLONG},
};

int  verify_unlink(unsigned int n)
{
	struct test_case_t *tc = &tcases[n];
	TST_EXP_FAIL(unlink(tc->name), tc->exp_errno, "%s", tc->desc);
}

void setup(void)
{
	size_t n;
	touch("file", 0777, NULL);
	memset(longpathname, 'a', PATH_MAX + 2);
	for (n = 0; n < ARRAY_SIZE(tcases); n++) {
		if (!tcases[n].name)
			tcases[n].name = tst_get_bad_addr(NULL);
	}
}

void main(){
	setup();
	cleanup();
}
