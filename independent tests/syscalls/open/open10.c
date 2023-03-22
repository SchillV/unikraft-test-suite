#include "incl.h"
#define MODE_RWX        0777
#define MODE_SGID       (S_ISGID|0777)
#define DIR_A		"dir_a"
#define DIR_B		"dir_b"
#define SETGID_A	DIR_A "/setgid"
#define NOSETGID_A	DIR_A "/nosetgid"
#define SETGID_B	DIR_B "/setgid"
#define NOSETGID_B	DIR_B "/nosetgid"
#define ROOT_SETGID	DIR_B "/root_setgid"

char *tmpdir;

uid_t orig_uid, nobody_uid;

gid_t nobody_gid, free_gid;

int fd = -1;

void setup(void)
{
	struct passwd *ltpuser = getpwnam("nobody");
	orig_uid = getuid();
	nobody_uid = ltpuser->pw_uid;
	nobody_gid = ltpuser->pw_gid;
	tst_res(TINFO, "User nobody: uid = %d, gid = %d", (int)nobody_uid,
		(int)nobody_gid);
	free_gid = tst_get_free_gid(nobody_gid);
	tmpdir = tst_get_tmpdir();
}

void file_test(const char *name, mode_t mode, int sgid, gid_t gid)
{
	struct stat buf;
	fd = open(name, O_CREAT | O_EXCL | O_RDWR, mode);
	close(fd);
	stat(name, &buf);
	if (buf.st_gid != gid) {
		tst_res(TFAIL, "%s: Incorrect group, %u != %u", name,
			buf.st_gid, gid);
	} else {
		tst_res(TPASS, "%s: Owned by correct group", name);
	}
	if (sgid < 0) {
		tst_res(TINFO, "%s: Skipping setgid bit check", name);
		return;
	}
	if (buf.st_mode & S_ISGID)
		tst_res(sgid ? TPASS : TFAIL, "%s: Setgid bit is set", name);
	else
		tst_res(sgid ? TFAIL : TPASS, "%s: Setgid bit not set", name);
}

void run(void)
{
	struct stat buf;
	mkdir(DIR_A, MODE_RWX);
	chown(DIR_A, nobody_uid, free_gid);
	stat(DIR_A, &buf);
	if (buf.st_mode & S_ISGID)
		tst_brk(TBROK, "%s: Setgid bit is set", DIR_A);
	if (buf.st_gid != free_gid) {
		tst_brk(TBROK, "%s: Incorrect group, %u != %u", DIR_A,
			buf.st_gid, free_gid);
	}
	mkdir(DIR_B, MODE_RWX);
	chown(DIR_B, nobody_uid, free_gid);
	chmod(DIR_B, MODE_SGID);
	stat(DIR_B, &buf);
	if (!(buf.st_mode & S_ISGID))
		tst_brk(TBROK, "%s: Setgid bit not set", DIR_B);
	if (buf.st_gid != free_gid) {
		tst_brk(TBROK, "%s: Incorrect group, %u != %u", DIR_B,
			buf.st_gid, free_gid);
	}
	setgid(nobody_gid);
	setreuid(-1, nobody_uid);
	file_test(NOSETGID_A, MODE_RWX, 0, nobody_gid);
	file_test(SETGID_A, MODE_SGID, 1, nobody_gid);
	file_test(NOSETGID_B, MODE_RWX, 0, free_gid);
	 * CVE 2018-13405 (privilege escalation using setgid bit) has its
	 * own test, skip setgid check here
	 */
	file_test(SETGID_B, MODE_SGID, -1, free_gid);
	setreuid(-1, orig_uid);
	file_test(ROOT_SETGID, MODE_SGID, 1, free_gid);
	tst_purge_dir(tmpdir);
}

void cleanup(void)
{
	if (fd >= 0)
		close(fd);
	free(tmpdir);
}

void main(){
	setup();
	cleanup();
}
