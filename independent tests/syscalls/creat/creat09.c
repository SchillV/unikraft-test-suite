#include "incl.h"
#define MODE_RWX        0777
#define MODE_SGID       (S_ISGID|0777)
#define MNTPOINT	"mntpoint"
#define WORKDIR		MNTPOINT "/testdir"
#define CREAT_FILE	WORKDIR "/creat.tmp"
#define OPEN_FILE	WORKDIR "/open.tmp"

gid_t free_gid;

int fd = -1;

struct tcase {
	const char *msg;
	int mask;
} tcases[] = {
	{"umask(0)", 0},
	{"umask(S_IXGRP)", S_IXGRP}
};

void setup(void)
{
	struct stat buf;
	struct passwd *ltpuser = getpwnam("nobody");
	tst_res(TINFO, "User nobody: uid = %d, gid = %d", (int)ltpuser->pw_uid,
		(int)ltpuser->pw_gid);
	free_gid = tst_get_free_gid(ltpuser->pw_gid);
	mkdir(WORKDIR, MODE_RWX);
	chown(WORKDIR, ltpuser->pw_uid, free_gid);
	chmod(WORKDIR, MODE_SGID);
	stat(WORKDIR, &buf);
	if (!(buf.st_mode & S_ISGID))
		tst_brk(TBROK, "%s: Setgid bit not set", WORKDIR);
	if (buf.st_gid != free_gid) {
		tst_brk(TBROK, "%s: Incorrect group, %u != %u", WORKDIR,
			buf.st_gid, free_gid);
	}
	setgid(ltpuser->pw_gid);
	setreuid(-1, ltpuser->pw_uid);
}

void file_test(const char *name)
{
	struct stat buf;
	stat(name, &buf);
	if (buf.st_gid != free_gid) {
		tst_res(TFAIL, "%s: Incorrect group, %u != %u", name,
			buf.st_gid, free_gid);
	} else {
		tst_res(TPASS, "%s: Owned by correct group", name);
	}
	if (buf.st_mode & S_ISGID)
		tst_res(TFAIL, "%s: Setgid bit is set", name);
	else
		tst_res(TPASS, "%s: Setgid bit not set", name);
}

void run(unsigned int n)
{
	struct tcase *tc = &tcases[n];
	umask(tc->mask);
	tst_res(TINFO, "File created with %s", tc->msg);
	fd = creat(CREAT_FILE, MODE_SGID);
	close(fd);
	file_test(CREAT_FILE);
	fd = open(OPEN_FILE, O_CREAT | O_EXCL | O_RDWR, MODE_SGID);
	file_test(OPEN_FILE);
	close(fd);
	tst_purge_dir(WORKDIR);
}

void cleanup(void)
{
	if (fd >= 0)
		close(fd);
}

void main(){
	setup();
	cleanup();
}
