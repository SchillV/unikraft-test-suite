#include "incl.h"
#define NOBODY_USER     99
#define MODE_TO1 S_IRUSR|S_IWUSR|S_IXUSR|S_IRGRP|S_IXGRP|S_IXOTH|S_IROTH
#define MODE_TO2 S_IRUSR|S_IWUSR|S_IXUSR|S_IRGRP|S_IWGRP|S_IXGRP|S_IXOTH|S_IROTH|S_IWOTH
#define MODE_TE S_IRUSR|S_IWUSR|S_IXUSR|S_IRGRP|S_IWGRP|S_IROTH|S_IWOTH
#define MODE_RWX        S_IRWXU | S_IRWXG | S_IRWXO
#define DIR_TEMP        "dir2/testdir_1"

char longpath[PATH_MAX + 2];

struct tcase {
	char *file1;
	char *desc1;
	char *file2;
	char *desc2;
	int exp_errno;
} tcases[] = {
	{"nonexistfile", "non-existent file", "nefile", "nefile", ENOENT},
	{"", "path is empty string", "nefile", "nefile", ENOENT},
	{"neefile/file", "path contains a non-existent file", "nefile", "nefile", ENOENT},
	{"regfile/file", "path contains a regular file", "nefile", "nefile", ENOTDIR},
	{longpath, "pathname too long", "nefile", "nefile", ENAMETOOLONG},
	{NULL, "invalid address", "nefile", "nefile", EFAULT},
	{"regfile", "regfile", "", "empty string", ENOENT},
	{"regfile", "regfile", "neefile/file", "path contains a non-existent file", ENOENT},
	{"regfile", "regfile", "file/file", "path contains a regular file", ENOENT},
	{"regfile", "regfile", longpath, "pathname too long", ENAMETOOLONG},
	{"regfile", "regfile", NULL, "invalid address", EFAULT},
	{"regfile", "regfile", "regfile2", "regfile2", EEXIST},
	{"dir1/oldpath", "Write access diretory", "dir1/newpath", "newpath", EACCES},
	{"dir2/testdir_1/tfile_2", "Search access diretory", "dir2/testdir_1/new_tfile_2",
						"dir2/testdir_1/new_tfile_2", EACCES},
};

int  verify_link(unsigned int i)
{
	struct passwd *nobody_pwd;
	struct tcase *tc = &tcases[i];
	if (tc->exp_errno == EACCES) {
		seteuid(0);
		if (strcmp(tc->desc1, "Write access diretory") == 0) {
			chmod("dir1", MODE_TO1);
			touch(tc->file1, 0777, NULL);
		} else if (strcmp(tc->desc1, "Search access diretory") == 0) {
			chmod("dir2", MODE_TO2);
			touch(tc->file1, 0666, NULL);
			chmod(DIR_TEMP, MODE_TE);
		}
		nobody_pwd = getpwnam("nobody");
		seteuid(nobody_pwd->pw_uid);
	}
link(tc->file1, tc->file2);
	if (TST_RET == -1) {
		if (TST_ERR == tc->exp_errno) {
			tst_res(TPASS | TTERRNO,
				"link(<%s>, <%s>)",
				tc->desc1, tc->desc2);
		} else {
			tst_res(TFAIL | TTERRNO,
				"link(<%s>, <%s>) Failed "
				"expected errno: %d",
				tc->desc1, tc->desc2,
				tc->exp_errno);
		}
	} else {
		tst_res(TFAIL,
			 "link(<%s>, <%s>) returned %ld, "
			"expected -1, errno:%d",
			tc->desc1, tc->desc2, TST_RET,
			tc->exp_errno);
	}
}

void setup(void)
{
	unsigned int i;
	memset(longpath, 'a', PATH_MAX+1);
	touch("regfile", 0777, NULL);
	touch("regfile2", 0777, NULL);
	mkdir("dir", 0777);
	mkdir("dir1", MODE_RWX);
	mkdir("dir2", MODE_RWX);
	mkdir(DIR_TEMP, MODE_RWX);
	for (i = 0; i < ARRAY_SIZE(tcases); i++) {
		struct tcase *tc = &tcases[i];
		if (!tc->file1)
			tc->file1 = tst_get_bad_addr(NULL);
		if (!tc->file2)
			tc->file2 = tst_get_bad_addr(NULL);
	}
}

void main(){
	setup();
	cleanup();
}
