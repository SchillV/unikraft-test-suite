#include "incl.h"
#define FILE_SIZE	 1024
#define TST_FILEREAD     "test_fileread"
#define TST_FILENOREAD   "test_filenoread"
#define READ_MODE        0666
#define NEW_MODE         0222
#define MASK             0777

uid_t user_id;

gid_t group_id;

struct passwd *ltpuser;

struct tcase{
	char *pathname;
	unsigned int mode;
} TC[] = {
	{TST_FILEREAD, READ_MODE},
	{TST_FILENOREAD, NEW_MODE}
};

int  verify_stat(unsigned int n)
{
	struct tcase *tc = TC + n;
	struct stat stat_buf;
	TST_EXP_PASS(stat(tc->pathname, &stat_buf));
	TST_EXP_EQ_LU(stat_buf.st_uid, user_id);
	TST_EXP_EQ_LU(stat_buf.st_gid, group_id);
	TST_EXP_EQ_LI(stat_buf.st_size, FILE_SIZE);
	TST_EXP_EQ_LU(stat_buf.st_mode & MASK, tc->mode);
	TST_EXP_EQ_LU(stat_buf.st_nlink, 1);
}

void setup(void)
{
	unsigned int i;
	ltpuser = getpwnam("nobody");
	setuid(ltpuser->pw_uid);
	for (i = 0; i < ARRAY_SIZE(TC); i++) {
		if (tst_fill_file(TC[i].pathname, 'a', 256, 4))
			tst_brk(TBROK, "Failed to create tst file %s",
				TC[i].pathname);
		chmod(TC[i].pathname, TC[i].mode);
	}
	user_id = getuid();
	group_id = getgid();
}

void main(){
	setup();
	cleanup();
}
