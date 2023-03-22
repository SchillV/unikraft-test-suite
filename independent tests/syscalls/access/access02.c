#include "incl.h"
#define FNAME_F	"file_f"
#define FNAME_R	"file_r"
#define FNAME_W	"file_w"
#define FNAME_X	"file_x"
#define SNAME_F	"symlink_f"
#define SNAME_R	"symlink_r"
#define SNAME_W	"symlink_w"
#define SNAME_X	"symlink_x"

uid_t uid;

struct tcase {
	const char *pathname;
	int mode;
	char *name;
	const char *targetname;
} tcases[] = {
	{FNAME_F, F_OK, "F_OK", FNAME_F},
	{FNAME_R, R_OK, "R_OK", FNAME_R},
	{FNAME_W, W_OK, "W_OK", FNAME_W},
	{FNAME_X, X_OK, "X_OK", FNAME_X},
	{SNAME_F, F_OK, "F_OK", FNAME_F},
	{SNAME_R, R_OK, "R_OK", FNAME_R},
	{SNAME_W, W_OK, "W_OK", FNAME_W},
	{SNAME_X, X_OK, "X_OK", FNAME_X}
};

void access_test(struct tcase *tc, const char *user)
{
	struct stat stat_buf;
	char command[64];
	TST_EXP_PASS_SILENT(access(tc->pathname, tc->mode),
	             "access(%s, %s) as %s", tc->pathname, tc->name, user);
	if (!TST_PASS)
		return;
	switch (tc->mode) {
	case F_OK:
		 * The specified file(or pointed to by symbolic link)
		 * exists, attempt to get its status, if successful,
		 * access() behaviour is correct.
		 */
stat(tc->targetname, &stat_buf);
		if (TST_RET == -1) {
			tst_res(TFAIL | TTERRNO, "stat(%s) as %s failed",
				tc->targetname, user);
			return;
		}
		break;
	case R_OK:
		 * The specified file(or pointed to by symbolic link)
		 * has read access, attempt to open the file with O_RDONLY,
		 * if we get a valid fd, access() behaviour is correct.
		 */
open(tc->targetname, O_RDONLY);
		if (TST_RET == -1) {
			tst_res(TFAIL | TTERRNO,
				"open %s with O_RDONLY as %s failed",
				tc->targetname, user);
			return;
		}
		close(TST_RET);
		break;
	case W_OK:
		 * The specified file(or pointed to by symbolic link)
		 * has write access, attempt to open the file with O_WRONLY,
		 * if we get a valid fd, access() behaviour is correct.
		 */
open(tc->targetname, O_WRONLY);
		if (TST_RET == -1) {
			tst_res(TFAIL | TTERRNO,
				"open %s with O_WRONLY as %s failed",
				tc->targetname, user);
			return;
		}
		close(TST_RET);
		break;
	case X_OK:
		 * The specified file(or pointed to by symbolic link)
		 * has execute access, attempt to execute the executable
		 * file, if successful, access() behaviour is correct.
		 */
		sprintf(command, "./%s", tc->targetname);
system(command);
		if (TST_RET != 0) {
			tst_res(TFAIL | TTERRNO, "execute %s as %s failed",
				tc->targetname, user);
			return;
		}
		break;
	default:
		break;
	}
	tst_res(TPASS, "access(%s, %s) as %s behaviour is correct.",
		tc->pathname, tc->name, user);
}

int  verify_access(unsigned int n)
{
	struct tcase *tc = &tcases[n];
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
	touch(FNAME_F, 0000, NULL);
	touch(FNAME_R, 0444, NULL);
	touch(FNAME_W, 0222, NULL);
	touch(FNAME_X, 0555, NULL);
	fprintf(FNAME_X, "#!%s\n", _PATH_BSHELL);
	symlink(FNAME_F, SNAME_F);
	symlink(FNAME_R, SNAME_R);
	symlink(FNAME_W, SNAME_W);
	symlink(FNAME_X, SNAME_X);
}

void main(){
	setup();
	cleanup();
}
