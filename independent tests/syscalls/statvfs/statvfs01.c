#include "incl.h"
#define MNT_POINT "mntpoint"
#define TEST_PATH MNT_POINT"/testfile"
#define NLS_MAX_CHARSET_SIZE 6

void run(void)
{
	struct statvfs buf;
	char valid_fname[PATH_MAX], toolong_fname[PATH_MAX];
	long fs_type;
	long valid_len;
	fs_type = tst_fs_type(TEST_PATH);
	TST_EXP_PASS(statvfs(TEST_PATH, &buf));
	valid_len = buf.f_namemax;
	if (fs_type == TST_VFAT_MAGIC || fs_type == TST_EXFAT_MAGIC)
		valid_len = buf.f_namemax / NLS_MAX_CHARSET_SIZE;
	memset(valid_fname, 'a', valid_len);
	memset(toolong_fname, 'b', valid_len + 1);
	valid_fname[valid_len] = 0;
	toolong_fname[valid_len+1] = 0;
	TST_EXP_FD(creat(valid_fname, 0444));
	if (TST_PASS)
		close(TST_RET);
	TST_EXP_FAIL(creat(toolong_fname, 0444), ENAMETOOLONG);
}

void setup(void)
{
	touch(TEST_PATH, 0666, NULL);
}

void main(){
	setup();
	cleanup();
}
