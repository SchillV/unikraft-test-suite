#include "incl.h"
#define TEMP_FILE	"tmp_file"
#define MNT_POINT	"mntpoint"
#define FILE_MODE	0644
#define TEST_USERNAME "nobody"

const struct utimbuf times;

struct tcase {
	char *pathname;
	int exp_errno;
	const struct utimbuf *utimbuf;
	char *err_desc;
} tcases[] = {
	{TEMP_FILE, EACCES, NULL, "No write access"},
	{"", ENOENT, NULL, "File not exist"},
	{TEMP_FILE, EPERM, &times, "Not file owner"},
	{MNT_POINT, EROFS, NULL, "Read-only filesystem"}
};

void setup(void)
{
	struct passwd *pw;
	touch(TEMP_FILE, FILE_MODE, NULL);
	pw = getpwnam(TEST_USERNAME);
	tst_res(TINFO, "Switching effective user ID to user: %s", pw->pw_name);
	seteuid(pw->pw_uid);
}

void run(unsigned int i)
{
	struct tcase *tc = &tcases[i];
	TST_EXP_FAIL(utime(tc->pathname, tc->utimbuf),
				tc->exp_errno, "%s", tc->err_desc);
}

void main(){
	setup();
	cleanup();
}
