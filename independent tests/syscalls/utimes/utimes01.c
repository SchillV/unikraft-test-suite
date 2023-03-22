#include "incl.h"
#define MNT_POINT "mntpoint"
#define TESTFILE1 "testfile1"
#define TESTFILE2 "testfile2"
#define TESTFILE3 "mntpoint/file"
#define FILE_MODE (S_IRWXU | S_IRGRP | S_IXGRP | \
					S_IROTH | S_IXOTH)
#define DIR_MODE (S_IRWXU | S_IRWXG | S_IRWXO)

struct timeval a_tv[2] = { {0, 0}, {1000, 0} };

struct timeval m_tv[2] = { {1000, 0}, {0, 0} };

struct timeval tv[2] = { {1000, 0}, {2000, 0} };

struct tcase {
	char *pathname;
	struct timeval *times;
	int exp_errno;
} tcases[] = {
	{ TESTFILE1, a_tv, 0 },
	{ TESTFILE1, m_tv, 0 },
	{ TESTFILE2, NULL, EACCES },
	{ "notexistfile", tv, ENOENT },
	{ NULL, tv, EFAULT },
	{ TESTFILE2, tv, EPERM },
	{ TESTFILE3, tv, EROFS },
};

void setup(void)
{
	struct passwd *ltpuser = getpwnam("nobody");
	touch(TESTFILE2, FILE_MODE, NULL);
	seteuid(ltpuser->pw_uid);
	touch(TESTFILE1, FILE_MODE, NULL);
}

int  utimes_verify(unsigned int i)
{
	struct stat st;
	struct timeval tmp_tv[2];
	struct tcase *tc = &tcases[i];
	if (tc->exp_errno == 0) {
		stat(tc->pathname, &st);
		tmp_tv[0].tv_sec = st.st_atime;
		tmp_tv[0].tv_usec = 0;
		tmp_tv[1].tv_sec = st.st_mtime;
		tmp_tv[1].tv_usec = 0;
	}
utimes(tc->pathname, tc->times);
	if (TST_ERR == tc->exp_errno) {
		tst_res(TPASS | TTERRNO, "utimes() worked as expected");
	} else {
		tst_res(TFAIL | TTERRNO,
			"utimes() failed unexpectedly; expected: %d - %s",
			tc->exp_errno, tst_strerrno(tc->exp_errno));
	}
	if (TST_ERR == 0 && utimes(tc->pathname, tmp_tv) == -1)
		tst_brk(TBROK | TERRNO, "utimes() failed.");
}

void main(){
	setup();
	cleanup();
}
