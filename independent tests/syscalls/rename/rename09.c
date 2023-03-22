#include "incl.h"
#define SRCDIR   "srcdir"
#define DESTDIR  "destdir"
#define SRCFILE  SRCDIR "/file"
#define DESTFILE DESTDIR "/file"
#define PERMS    0700

uid_t orig_uid, test_users[2];

char *tmpdir;

void setup(void)
{
	umask(0);
	orig_uid = getuid();
	tst_get_uids(test_users, 0, 2);
	tmpdir = tst_get_tmpdir();
}

void run(void)
{
	gid_t curgid = getgid();
	mkdir(SRCDIR, PERMS);
	touch(SRCFILE, PERMS, NULL);
	chown(SRCDIR, test_users[0], curgid);
	chown(SRCFILE, test_users[0], curgid);
	seteuid(test_users[1]);
	mkdir(DESTDIR, PERMS);
	touch(DESTFILE, PERMS, NULL);
	TST_EXP_FAIL(rename(SRCFILE, DESTFILE), EACCES, "rename()");
	seteuid(orig_uid);
	tst_purge_dir(tmpdir);
}

void cleanup(void)
{
	free(tmpdir);
}

void main(){
	setup();
	cleanup();
}
