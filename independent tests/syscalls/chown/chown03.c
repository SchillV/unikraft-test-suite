#include "incl.h"
#define FILE_MODE (S_IFREG|S_IRUSR|S_IWUSR|S_IRGRP|S_IROTH)
#define NEW_PERMS (S_IFREG|S_IRWXU|S_IRWXG|S_ISUID|S_ISGID)
#define FILENAME "chown03_testfile"

struct passwd *ltpuser;

void check_owner(struct stat *s, uid_t exp_uid, gid_t exp_gid)
{
	if (s->st_uid != exp_uid || s->st_gid != exp_gid)
		tst_res(TFAIL, "%s: wrong owner set to (uid=%d, gid=%d),"
			       " expected (uid=%d, gid=%d)",
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
	seteuid(0);
	chown(FILENAME, -1, 0);
	chmod(FILENAME, NEW_PERMS);
	seteuid(ltpuser->pw_uid);
	uid_t uid;
	gid_t gid;
	UID16_CHECK((uid = geteuid()), "chown");
	GID16_CHECK((gid = getegid()), "chown");
	struct stat stat_buf;
	stat(FILENAME, &stat_buf);
	check_owner(&stat_buf, uid, 0);
	check_mode(&stat_buf, NEW_PERMS);
	TST_EXP_PASS(CHOWN(FILENAME, -1, gid), "chown(%s, %d, %d)",
		     FILENAME, -1, gid);
	stat(FILENAME, &stat_buf);
	check_owner(&stat_buf, uid, gid);
	check_mode(&stat_buf, NEW_PERMS & ~(S_ISUID | S_ISGID));
}

void setup(void)
{
	int fd;
	ltpuser = getpwnam("nobody");
	setegid(ltpuser->pw_gid);
	seteuid(ltpuser->pw_uid);
	fd = open(FILENAME, O_RDWR | O_CREAT, FILE_MODE);
	close(fd);
}

void cleanup(void)
{
	setegid(0);
	seteuid(0);
}

void main(){
	setup();
	cleanup();
}
