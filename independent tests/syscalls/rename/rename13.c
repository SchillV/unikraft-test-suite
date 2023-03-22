#include "incl.h"
#define MNT_POINT "mntpoint"
#define TEMP_FILE1 MNT_POINT"/tmpfile1"
#define TEMP_FILE2 MNT_POINT"/tmpfile2"

struct stat buf1, buf2;

void setup(void)
{
	touch(TEMP_FILE1, 0700, NULL);
	stat(TEMP_FILE1, &buf1);
	link(TEMP_FILE1, TEMP_FILE2);
}

void run(void)
{
	TST_EXP_PASS(rename(TEMP_FILE1, TEMP_FILE1));
	if (TST_RET != 0)
		return;
	stat(TEMP_FILE2, &buf2);
	TST_EXP_EQ_LU(buf1.st_dev, buf2.st_dev);
	TST_EXP_EQ_LU(buf1.st_ino, buf2.st_ino);
}

void main(){
	setup();
	cleanup();
}
