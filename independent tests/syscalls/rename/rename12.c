#include "incl.h"
#define MNT_POINT "mntpoint"
#define TEMP_DIR "tempdir"
#define TEMP_FILE1 TEMP_DIR"/tmpfile1"
#define TEMP_FILE2 TEMP_DIR"/tmpfile2"

uid_t nobody_uid;

struct stat buf1;

void setup(void)
{
	struct passwd *pw;
	pw = getpwnam("nobody");
	nobody_uid = pw->pw_uid;
	chdir(MNT_POINT);
	mkdir(TEMP_DIR, 0777);
	stat(TEMP_DIR, &buf1);
	chmod(TEMP_DIR, buf1.st_mode | S_ISVTX);
	touch(TEMP_FILE1, 0700, NULL);
}

void run(void)
{
	seteuid(nobody_uid);
rename(TEMP_FILE1, TEMP_FILE2);
	if (TST_RET == -1 && (TST_ERR == EPERM || TST_ERR == EACCES))
		tst_res(TPASS | TTERRNO, "rename() failed as expected");
	else if (TST_RET == 0)
		tst_res(TFAIL, "rename() succeeded unexpectedly");
	else
		tst_res(TFAIL | TTERRNO, "rename() failed, but not with expected errno");
}

void main(){
	setup();
	cleanup();
}
