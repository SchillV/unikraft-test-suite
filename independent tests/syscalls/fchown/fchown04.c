#include "incl.h"
#define MNT_POINT	"mntpoint"
#define TEST_FILE	"tfile_1"
#define MODE		0666
#define DIR_MODE	(S_IRUSR|S_IWUSR|S_IXUSR|S_IRGRP|S_IXGRP|S_IROTH|S_IXOTH)

int fd1;

int fd2 = -1;

int fd3;

struct test_case_t {
	int *fd;
	int exp_errno;
} tc[] = {
	{&fd1, EPERM},
	{&fd2, EBADF},
	{&fd3, EROFS},
};

void setup(void)
{
	struct passwd *ltpuser;
	fd1 = open(TEST_FILE, O_RDWR | O_CREAT, MODE);
	fd3 = open(MNT_POINT, O_RDONLY);
	ltpuser = getpwnam("nobody");
	seteuid(ltpuser->pw_uid);
}

void run(unsigned int i)
{
	uid_t uid;
	gid_t gid;
	UID16_CHECK((uid = geteuid()), "fchown");
	GID16_CHECK((gid = getegid()), "fchown");
	TST_EXP_FAIL(FCHOWN(*tc[i].fd, uid, gid), tc[i].exp_errno,
	             "fchown(%i, %i, %i)", *tc[i].fd, uid, gid);
}

void cleanup(void)
{
	if (fd1 > 0)
		close(fd1);
	if (fd3 > 0)
		close(fd3);
}

void main(){
	setup();
	cleanup();
}
