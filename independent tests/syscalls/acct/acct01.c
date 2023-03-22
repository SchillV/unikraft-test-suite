#include "incl.h"
#define DIR_MODE	(S_IRUSR|S_IWUSR|S_IXUSR|S_IRGRP| \
			 S_IXGRP|S_IROTH|S_IXOTH)
#define FILE_EISDIR		"."
#define FILE_EACCES		"/dev/null"
#define FILE_ENOENT		"/tmp/does/not/exist"
#define FILE_ENOTDIR		"./tmpfile/"
#define TEST_TMPFILE		"./tmpfile"
#define TEST_ELOOP		"test_file_eloop1"
#define TEST_ENAMETOOLONG	nametoolong
#define TEST_EROFS		"mntpoint/file"

char nametoolong[PATH_MAX+2];

struct passwd *ltpuser;

void setup_euid(void)
{
	seteuid(ltpuser->pw_uid);
}

void cleanup_euid(void)
{
	seteuid(0);
}

struct test_case {
	char *filename;
	char *exp_errval;
	int exp_errno;
	void (*setupfunc) ();
	void (*cleanfunc) ();
} tcases[] = {
	{FILE_EISDIR, "EISDIR",  EISDIR,  NULL,   NULL},
	{FILE_EACCES, "EACCES",  EACCES,  NULL,   NULL},
	{FILE_ENOENT, "ENOENT",  ENOENT,  NULL,   NULL},
	{FILE_ENOTDIR, "ENOTDIR", ENOTDIR, NULL,   NULL},
	{TEST_TMPFILE, "EPERM",   EPERM,   setup_euid, cleanup_euid},
	{NULL,       "EPERM",   EPERM,   setup_euid, cleanup_euid},
	{TEST_ELOOP, "ELOOP",        ELOOP,        NULL, NULL},
	{TEST_ENAMETOOLONG, "ENAMETOOLONG", ENAMETOOLONG, NULL, NULL},
	{TEST_EROFS, "EROFS",        EROFS,        NULL, NULL},
};

void setup(void)
{
	int fd;
acct(NULL);
	if (TST_RET == -1 && TST_ERR == ENOSYS)
		tst_brk(TCONF, "acct() system call isn't configured in kernel");
	ltpuser = getpwnam("nobody");
	fd = creat(TEST_TMPFILE, 0777);
	close(fd);
acct(TEST_TMPFILE);
	if (TST_RET == -1)
		tst_brk(TBROK | TTERRNO, "acct failed unexpectedly");
acct(NULL);
	if (TST_RET == -1)
		tst_brk(TBROK | TTERRNO, "acct(NULL) failed");
	symlink(TEST_ELOOP, "test_file_eloop2");
	symlink("test_file_eloop2", TEST_ELOOP);
	memset(nametoolong, 'a', PATH_MAX+1);
}

int  verify_acct(unsigned int nr)
{
	struct test_case *tcase = &tcases[nr];
	if (tcase->setupfunc)
		tcase->setupfunc();
acct(tcase->filename);
	if (tcase->cleanfunc)
		tcase->cleanfunc();
	if (TST_RET != -1) {
		tst_res(TFAIL, "acct(%s) succeeded unexpectedly",
				tcase->filename);
		return;
	}
	if (TST_ERR == tcase->exp_errno) {
		tst_res(TPASS | TTERRNO, "acct() failed as expected");
	} else {
		tst_res(TFAIL | TTERRNO,
				"acct() failed, expected: %s",
				tst_strerrno(tcase->exp_errno));
	}
}

void main(){
	setup();
	cleanup();
}
