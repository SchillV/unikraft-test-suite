#include "incl.h"
#define TEST_SYMLINK	"statvfs_symlink"
#define TEST_FILE	"statvfs_file"

struct statvfs buf;

char nametoolong[PATH_MAX+2];

struct tcase {
	char *path;
	struct statvfs *buf;
	int exp_errno;
} tcases[] = {
	{(char *)-1, &buf, EFAULT},
	{TEST_SYMLINK, &buf, ELOOP},
	{nametoolong, &buf, ENAMETOOLONG},
	{"filenoexist", &buf, ENOENT},
	{"statvfs_file/test", &buf, ENOTDIR},
};

void setup(void)
{
	unsigned int i;
	symlink(TEST_SYMLINK, "symlink_2");
	symlink("symlink_2", TEST_SYMLINK);
	memset(nametoolong, 'a', PATH_MAX+1);
	touch(TEST_FILE, 0644, NULL);
	for (i = 0; i < ARRAY_SIZE(tcases); i++) {
		if (tcases[i].path == (char *)-1)
			tcases[i].path = tst_get_bad_addr(NULL);
	}
}

void run(unsigned int i)
{
	struct tcase *tc = &tcases[i];
	TST_EXP_FAIL(statvfs(tc->path, tc->buf), tc->exp_errno);
}

void main(){
	setup();
	cleanup();
}
