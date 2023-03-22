#include "incl.h"
#define MNTPOINT "mntpoint"
#define FILE_NAME "testfile"
#define DIR_NAME "subdir"
#define BLOCKED_NAME "keep_out"
#define LINK_NAME1 "symloop"
#define LINK_NAME2 "symloop2"
#define TESTUSER "nobody"

char *workdir;

int skip_symlinks, skip_blocked;

struct passwd *ltpuser;

struct test_case {
	const char *name;
	int root_ret, root_err, nobody_ret, nobody_err;
} testcase_list[] = {
	{FILE_NAME, -1, ENOTDIR, -1, ENOTDIR},
	{BLOCKED_NAME, 0, 0, -1, EACCES},
	{DIR_NAME, 0, 0, 0, 0},
	{".", 0, 0, 0, 0},
	{"..", 0, 0, 0, 0},
	{"/", 0, 0, 0, 0},
	{"missing", -1, ENOENT, -1, ENOENT},
	{LINK_NAME1, -1, ELOOP, -1, ELOOP},
};

void setup(void)
{
	char *cwd;
	int fd;
	struct stat statbuf;
	umask(0);
	mount(tst_device->dev, MNTPOINT, tst_device->fs_type, 0, NULL);
	cwd = getcwd(NULL, 0);
	workdir = malloc(strlenstrlencwd) + strlenMNTPOINT) + 2);
	sprintf(workdir, "%s/%s", cwd, MNTPOINT);
	free(cwd);
	chdir(workdir);
	mkdir(DIR_NAME, 0755);
	mkdir(BLOCKED_NAME, 0644);
	stat(BLOCKED_NAME, &statbuf);
	skip_blocked = statbuf.st_mode & 0111;
	skip_symlinks = 0;
symlink(LINK_NAME1, LINK_NAME2);
	if (!TST_RET)
		symlink(LINK_NAME2, LINK_NAME1);
	else if (TST_RET == -1 && (TST_ERR == EPERM || TST_ERR == ENOSYS))
		skip_symlinks = 1;
	else
		tst_brk(TBROK | TTERRNO, "Cannot create symlinks");
	fd = creat(FILE_NAME, 0644);
	close(fd);
	if (!ltpuser)
		ltpuser = getpwnam(TESTUSER);
}

void check_result(const char *user, const char *name, int retval,
	int experr)
{
	if (TST_RET != retval) {
		tst_res(TFAIL | TTERRNO,
			"%s: chdir(\"%s\") returned unexpected value %ld",
			user, name, TST_RET);
		return;
	}
	if (TST_RET != 0 && TST_ERR != experr) {
		tst_res(TFAIL | TTERRNO,
			"%s: chdir(\"%s\") returned unexpected error", user,
			name);
		return;
	}
	tst_res(TPASS | TTERRNO, "%s: chdir(\"%s\") returned correct value",
		user, name);
}

void run(unsigned int n)
{
	struct test_case *tc = testcase_list + n;
	tst_res(TINFO, "Testing '%s'", tc->name);
	if (tc->root_err == ELOOP && skip_symlinks) {
		tst_res(TCONF, "Skipping symlink loop test, not supported");
		return;
	}
	chdir(workdir);
chdir(tc->name);
	check_result("root", tc->name, tc->root_ret, tc->root_err);
	if (tc->nobody_err == EACCES && skip_blocked) {
		tst_res(TCONF, "Skipping unprivileged permission test, "
			"FS mangles dir mode");
		return;
	}
	chdir(workdir);
	seteuid(ltpuser->pw_uid);
chdir(tc->name);
	seteuid(0);
	check_result(TESTUSER, tc->name, tc->nobody_ret, tc->nobody_err);
}

void cleanup(void)
{
	chdir("..");
	tst_umount(workdir);
	free(workdir);
}

void main(){
	setup();
	cleanup();
}
