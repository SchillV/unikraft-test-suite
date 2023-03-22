#include "incl.h"
#define MNT_POINT	"mntpoint"
#define TEMP_FILE	MNT_POINT"/tmp_file"
#define FILE_MODE	0444
#define NEW_MODF_TIME	10000
#define NEW_ACCESS_TIME	20000

struct utimbuf times = {
	.modtime = NEW_MODF_TIME,
	.actime = NEW_ACCESS_TIME
};

void setup(void)
{
	touch(TEMP_FILE, FILE_MODE, NULL);
}

void run(void)
{
	struct stat stat_buf;
	TST_EXP_PASS(utime(TEMP_FILE, &times), "utime(%s, &times)", TEMP_FILE);
	if (!TST_PASS)
		return;
	stat(TEMP_FILE, &stat_buf);
	TST_EXP_EQ_LI(stat_buf.st_mtime, NEW_MODF_TIME);
	TST_EXP_EQ_LI(stat_buf.st_atime, NEW_ACCESS_TIME);
}

void main(){
	setup();
	cleanup();
}
