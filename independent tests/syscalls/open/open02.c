#include "incl.h"
#define _GNU_SOURCE
#define TEST_FILE	"test_file"
#define TEST_FILE2	"test_file2"

struct tcase {
	const char *filename;
	int flag;
	int exp_errno;
	const char *desc;
} tcases[] = {
	{TEST_FILE, O_RDWR, ENOENT, "new file without O_CREAT"},
	{TEST_FILE2, O_RDONLY | O_NOATIME, EPERM, "unprivileged O_RDONLY | O_NOATIME"},
};
void setup(void)
{
	struct passwd *ltpuser;
	touch(TEST_FILE2, 0644, NULL);
	ltpuser = getpwnam("nobody");
	seteuid(ltpuser->pw_uid);
}

int  verify_open(unsigned int n)
{
	struct tcase *tc = &tcases[n];
	TST_EXP_FAIL2(open(tc->filename, tc->flag, 0444),
	             tc->exp_errno, "open() %s", tc->desc);
}
void cleanup(void)
{
	seteuid(0);
}

void main(){
	setup();
	cleanup();
}
