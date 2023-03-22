#include "incl.h"
#define FILE_MODE	(S_IFREG|S_IRUSR|S_IWUSR|S_IRGRP|S_IROTH)
#define NEW_PERMS1	(S_IFREG|S_IRWXU|S_IRWXG|S_ISUID|S_ISGID)
#define NEW_PERMS2	(S_IFREG|S_IRWXU|S_ISGID)
#define EXP_PERMS	(S_IFREG|S_IRWXU|S_IRWXG)
#define TESTFILE1	"testfile1"
#define TESTFILE2	"testfile2"

int fd1, fd2;
struct test_case_t {
	int *fd;
	const char *filename;
	mode_t set_mode;
	mode_t exp_mode;
} tc[] = {
	{&fd1, TESTFILE1, NEW_PERMS1, EXP_PERMS},
	{&fd2, TESTFILE2, NEW_PERMS2, NEW_PERMS2}
};

void run(unsigned int i)
{
	struct stat stat_buf;
	uid_t uid;
	gid_t gid;
	UID16_CHECK((uid = geteuid()), "fchown");
	GID16_CHECK((gid = getegid()), "fchown");
	chmod(tc[i].filename, tc[i].set_mode);
	TST_EXP_PASS(FCHOWN(*tc[i].fd, uid, gid),
		"fchown(%i, %i, %i)", *tc[i].fd, uid, gid);
	stat(tc[i].filename, &stat_buf);
	if (stat_buf.st_uid != uid || stat_buf.st_gid != gid)
		tst_res(TFAIL, "%s: owner set to (uid=%d, gid=%d), expected (uid=%d, gid=%d)",
			tc[i].filename, stat_buf.st_uid, stat_buf.st_gid, uid, gid);
	if (stat_buf.st_mode != tc[i].exp_mode)
		tst_res(TFAIL, "%s: wrong mode permissions %#o, expected %#o",
			tc[i].filename, stat_buf.st_mode, tc[i].exp_mode);
}

void setup(void)
{
	unsigned int i;
	for (i = 0; i < ARRAY_SIZE(tc); i++)
		*tc[i].fd = open(tc[i].filename, O_RDWR | O_CREAT, FILE_MODE);
}

void cleanup(void)
{
	unsigned int i;
	for (i = 0; i < ARRAY_SIZE(tc); i++) {
		if (*tc[i].fd > 0)
			close(*tc[i].fd);
	}
}

void main(){
	setup();
	cleanup();
}
