#include "incl.h"
#define FILE_MODE   S_IRUSR | S_IWUSR | S_IRGRP | S_IROTH
#define DIR_MODE	S_IRWXU | S_IRWXG | S_IRWXO
#define PERMS		01777
#define TESTFILE	"testfile"
#define TESTDIR		"testdir_3"

struct tcase {
	char *name;
	char *desc;
} tcases[] = {
int 	{TESTFILE, "verify permissions of file"},
int 	{TESTDIR, "verify permissions of directory"},
};

int  verify_chmod(unsigned int n)
{
	struct stat stat_buf;
	struct tcase *tc = &tcases[n];
	TST_EXP_PASS(chmod(tc->name, PERMS), "chmod(%s, %04o)",
		tc->name, PERMS);
	if (!TST_PASS)
		return;
	stat(tc->name, &stat_buf);
	if ((stat_buf.st_mode & PERMS) != PERMS) {
		tst_res(TFAIL, "stat(%s) mode=%04o",
			tc->name, stat_buf.st_mode);
	} else {
		tst_res(TPASS, "stat(%s) mode=%04o",
			tc->name, stat_buf.st_mode);
	}
}

void setup(void)
{
	char nobody_uid[] = "nobody";
	struct passwd *ltpuser;
	ltpuser = getpwnam(nobody_uid);
	seteuid(ltpuser->pw_uid);
	touch(TESTFILE, FILE_MODE, NULL);
	mkdir(TESTDIR, DIR_MODE);
}

void main(){
	setup();
	cleanup();
}
