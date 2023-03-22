#include "incl.h"
#define FILE_MODE	(S_IFREG|S_IRUSR|S_IWUSR|S_IRGRP|S_IROTH)
#define NEW_PERMS	(S_IFREG|S_IRWXU|S_IRWXG|S_ISUID|S_ISGID)
#define FILENAME	"fchown03_testfile"

int fd;

struct passwd *ltpuser;

void check_owner(struct stat *s, uid_t exp_uid, gid_t exp_gid)
{
	if (s->st_uid != exp_uid || s->st_gid != exp_gid)
		tst_res(TFAIL, "%s: wrong owner set to (uid=%d, gid=%d), expected (uid=%d, gid=%d)",
			FILENAME, s->st_uid, s->st_gid, exp_uid, exp_gid);
}

void check_mode(struct stat *s, mode_t exp_mode)
{
	if (s->st_mode != exp_mode)
		tst_res(TFAIL, "%s: wrong mode permissions %#o, expected %#o",
			FILENAME, s->st_mode, exp_mode);
}

void run(void)
{
	uid_t uid;
	gid_t gid;
	struct stat stat_buf;
	seteuid(0);
	fchown(fd, -1, 0);
	fchmod(fd, NEW_PERMS);
	seteuid(ltpuser->pw_uid);
	UID16_CHECK((uid = geteuid()), "fchown");
	GID16_CHECK((gid = getegid()), "fchown");
	stat(FILENAME, &stat_buf);
	check_owner(&stat_buf, uid, 0);
	check_mode(&stat_buf, NEW_PERMS);
	TST_EXP_PASS(FCHOWN(fd, -1, gid), "fchown(fd, %d, %d)", -1, gid);
	stat(FILENAME, &stat_buf);
	check_owner(&stat_buf, uid, gid);
	check_mode(&stat_buf, NEW_PERMS & ~(S_ISUID | S_ISGID));
}

void setup(void)
{
	ltpuser = getpwnam("nobody");
	setegid(ltpuser->pw_gid);
	seteuid(ltpuser->pw_uid);
	fd = open(FILENAME, O_RDWR | O_CREAT, FILE_MODE);
}

void cleanup(void)
{
	setegid(0);
	seteuid(0);
	if (fd > 0)
		close(fd);
}

void main(){
	setup();
	cleanup();
}
