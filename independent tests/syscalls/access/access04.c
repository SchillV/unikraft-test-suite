#include "incl.h"
#define FNAME1	"accessfile1"
#define FNAME2	"accessfile2/accessfile2"
#define DNAME	"accessfile2"
#define SNAME1	"symlink1"
#define SNAME2	"symlink2"
#define MNT_POINT	"mntpoint"

uid_t uid;

char longpathname[PATH_MAX + 2];

struct tcase {
	const char *pathname;
	int mode;
	int exp_errno;
} tcases[] = {
	{FNAME1, -1, EINVAL},
	{"", W_OK, ENOENT},
	{longpathname, R_OK, ENAMETOOLONG},
	{FNAME2, R_OK, ENOTDIR},
	{SNAME1, R_OK, ELOOP},
	{MNT_POINT, W_OK, EROFS}
};

void access_test(struct tcase *tc, const char *user)
{
	TST_EXP_FAIL(access(tc->pathname, tc->mode), tc->exp_errno,
	             "access as %s", user);
}

int  verify_access(unsigned int n)
{
	struct tcase *tc = tcases + n;
	pid_t pid;
	access_test(tc, "root");
	pid = fork();
	if (pid) {
		waitpid(pid, NULL, 0);
	} else {
		setuid(uid);
		access_test(tc, "nobody");
	}
}

void setup(void)
{
	struct passwd *pw;
	pw = getpwnam("nobody");
	uid = pw->pw_uid;
	memset(longpathname, 'a', sizeof(longpathname) - 1);
	touch(FNAME1, 0333, NULL);
	touch(DNAME, 0644, NULL);
	symlink(SNAME1, SNAME2);
	symlink(SNAME2, SNAME1);
}

void main(){
	setup();
	cleanup();
}
