#include "incl.h"
#define MNT_POINT	"mntpoint"
#define TEMP_FILE	MNT_POINT"/tmp_file"
#define FILE_MODE	0444

void setup(void)
{
	touch(TEMP_FILE, FILE_MODE, NULL);
}

void run(void)
{
	struct utimbuf utbuf;
	struct stat stat_buf;
	time_t pre_time, post_time;
	utbuf.modtime = tst_get_fs_timestamp() - 5;
	utbuf.actime = utbuf.modtime + 1;
	TST_EXP_PASS_SILENT(utime(TEMP_FILE, &utbuf));
	stat(TEMP_FILE, &stat_buf);
	TST_EXP_EQ_LI(stat_buf.st_atime, utbuf.actime);
	TST_EXP_EQ_LI(stat_buf.st_mtime, utbuf.modtime);
	pre_time = tst_get_fs_timestamp();
	TST_EXP_PASS(utime(TEMP_FILE, NULL), "utime(%s, NULL)", TEMP_FILE);
	if (!TST_PASS)
		return;
	post_time = tst_get_fs_timestamp();
	stat(TEMP_FILE, &stat_buf);
	if (stat_buf.st_mtime < pre_time || stat_buf.st_mtime > post_time)
		tst_res(TFAIL, "utime() did not set expected mtime, "
				"pre_time: %ld, post_time: %ld, st_mtime: %ld",
				pre_time, post_time, stat_buf.st_mtime);
	if (stat_buf.st_atime < pre_time || stat_buf.st_atime > post_time)
		tst_res(TFAIL, "utime() did not set expected atime, "
				"pre_time: %ld, post_time: %ld, st_atime: %ld",
				pre_time, post_time, stat_buf.st_atime);
}

void main(){
	setup();
	cleanup();
}
