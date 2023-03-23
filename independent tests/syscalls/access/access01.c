#include "incl.h"
#define FNAME_RWX "accessfile_rwx"
#define FNAME_R   "accessfile_r"
#define FNAME_W   "accessfile_w"
#define FNAME_X   "accessfile_x"
#define DNAME_R   "accessdir_r"
#define DNAME_W   "accessdir_w"
#define DNAME_X   "accessdir_x"
#define DNAME_RW  "accessdir_rw"
#define DNAME_RX  "accessdir_rx"
#define DNAME_WX  "accessdir_wx"

uid_t uid;

struct tcase {
	const char *fname;
	int mode;
	char *name;
	int exp_errno;
	int exp_user;
} tcases[] = {
	{FNAME_RWX, F_OK, "F_OK", 0, 3},
	{FNAME_RWX, X_OK, "X_OK", 0, 3},
	{FNAME_RWX, W_OK, "W_OK", 0, 3},
	{FNAME_RWX, R_OK, "R_OK", 0, 3},
	{FNAME_RWX, R_OK|W_OK, "R_OK|W_OK", 0, 3},
	{FNAME_RWX, R_OK|X_OK, "R_OK|X_OK", 0, 3},
	{FNAME_RWX, W_OK|X_OK, "W_OK|X_OK", 0, 3},
	{FNAME_RWX, R_OK|W_OK|X_OK, "R_OK|W_OK|X_OK", 0, 3},
	{FNAME_X, X_OK, "X_OK", 0, 3},
	{FNAME_W, W_OK, "W_OK", 0, 3},
	{FNAME_R, R_OK, "R_OK", 0, 3},
	{FNAME_R, X_OK, "X_OK", EACCES, 3},
	{FNAME_R, W_OK, "W_OK", EACCES, 1},
	{FNAME_W, R_OK, "R_OK", EACCES, 1},
	{FNAME_W, X_OK, "X_OK", EACCES, 3},
	{FNAME_X, R_OK, "R_OK", EACCES, 1},
	{FNAME_X, W_OK, "W_OK", EACCES, 1},
	{FNAME_R, W_OK|X_OK, "W_OK|X_OK", EACCES, 3},
	{FNAME_R, R_OK|X_OK, "R_OK|X_OK", EACCES, 3},
	{FNAME_R, R_OK|W_OK, "R_OK|W_OK", EACCES, 1},
	{FNAME_R, R_OK|W_OK|X_OK, "R_OK|W_OK|X_OK", EACCES, 3},
	{FNAME_W, W_OK|X_OK, "W_OK|X_OK", EACCES, 3},
	{FNAME_W, R_OK|X_OK, "R_OK|X_OK", EACCES, 3},
	{FNAME_W, R_OK|W_OK, "R_OK|W_OK", EACCES, 1},
	{FNAME_W, R_OK|W_OK|X_OK, "R_OK|W_OK|X_OK", EACCES, 3},
	{FNAME_X, W_OK|X_OK, "W_OK|X_OK", EACCES, 1},
	{FNAME_X, R_OK|X_OK, "R_OK|X_OK", EACCES, 1},
	{FNAME_X, R_OK|W_OK, "R_OK|W_OK", EACCES, 1},
	{FNAME_X, R_OK|W_OK|X_OK, "R_OK|W_OK|X_OK", EACCES, 1},
	{FNAME_R, W_OK, "W_OK", 0, 2},
	{FNAME_R, R_OK|W_OK, "R_OK|W_OK", 0, 2},
	{FNAME_W, R_OK, "R_OK", 0, 2},
	{FNAME_W, R_OK|W_OK, "R_OK|W_OK", 0, 2},
	{FNAME_X, R_OK, "R_OK", 0, 2},
	{FNAME_X, W_OK, "W_OK", 0, 2},
	{FNAME_X, R_OK|W_OK, "R_OK|W_OK", 0, 2},
	{DNAME_R"/"FNAME_R, F_OK, "F_OK", 0, 2},
	{DNAME_R"/"FNAME_R, R_OK, "R_OK", 0, 2},
	{DNAME_R"/"FNAME_R, W_OK, "W_OK", 0, 2},
	{DNAME_R"/"FNAME_W, F_OK, "F_OK", 0, 2},
	{DNAME_R"/"FNAME_W, R_OK, "R_OK", 0, 2},
	{DNAME_R"/"FNAME_W, W_OK, "W_OK", 0, 2},
	{DNAME_R"/"FNAME_X, F_OK, "F_OK", 0, 2},
	{DNAME_R"/"FNAME_X, R_OK, "R_OK", 0, 2},
	{DNAME_R"/"FNAME_X, W_OK, "W_OK", 0, 2},
	{DNAME_R"/"FNAME_X, X_OK, "X_OK", 0, 2},
	{DNAME_W"/"FNAME_R, F_OK, "F_OK", 0, 2},
	{DNAME_W"/"FNAME_R, R_OK, "R_OK", 0, 2},
	{DNAME_W"/"FNAME_R, W_OK, "W_OK", 0, 2},
	{DNAME_W"/"FNAME_W, F_OK, "F_OK", 0, 2},
	{DNAME_W"/"FNAME_W, R_OK, "R_OK", 0, 2},
	{DNAME_W"/"FNAME_W, W_OK, "W_OK", 0, 2},
	{DNAME_W"/"FNAME_X, F_OK, "F_OK", 0, 2},
	{DNAME_W"/"FNAME_X, R_OK, "R_OK", 0, 2},
	{DNAME_W"/"FNAME_X, W_OK, "W_OK", 0, 2},
	{DNAME_W"/"FNAME_X, X_OK, "X_OK", 0, 2},
	{DNAME_X"/"FNAME_R, F_OK, "F_OK", 0, 3},
	{DNAME_X"/"FNAME_R, R_OK, "R_OK", 0, 3},
	{DNAME_X"/"FNAME_R, W_OK, "W_OK", 0, 2},
	{DNAME_X"/"FNAME_W, F_OK, "F_OK", 0, 3},
	{DNAME_X"/"FNAME_W, R_OK, "R_OK", 0, 2},
	{DNAME_X"/"FNAME_W, W_OK, "W_OK", 0, 3},
	{DNAME_X"/"FNAME_X, F_OK, "F_OK", 0, 3},
	{DNAME_X"/"FNAME_X, R_OK, "R_OK", 0, 2},
	{DNAME_X"/"FNAME_X, W_OK, "W_OK", 0, 2},
	{DNAME_X"/"FNAME_X, X_OK, "X_OK", 0, 3},
	{DNAME_RW"/"FNAME_R, F_OK, "F_OK", 0, 2},
	{DNAME_RW"/"FNAME_R, R_OK, "R_OK", 0, 2},
	{DNAME_RW"/"FNAME_R, W_OK, "W_OK", 0, 2},
	{DNAME_RW"/"FNAME_W, F_OK, "F_OK", 0, 2},
	{DNAME_RW"/"FNAME_W, R_OK, "R_OK", 0, 2},
	{DNAME_RW"/"FNAME_W, W_OK, "W_OK", 0, 2},
	{DNAME_RW"/"FNAME_X, F_OK, "F_OK", 0, 2},
	{DNAME_RW"/"FNAME_X, R_OK, "R_OK", 0, 2},
	{DNAME_RW"/"FNAME_X, W_OK, "W_OK", 0, 2},
	{DNAME_RW"/"FNAME_X, X_OK, "X_OK", 0, 2},
	{DNAME_RX"/"FNAME_R, F_OK, "F_OK", 0, 3},
	{DNAME_RX"/"FNAME_R, R_OK, "R_OK", 0, 3},
	{DNAME_RX"/"FNAME_R, W_OK, "W_OK", 0, 2},
	{DNAME_RX"/"FNAME_W, F_OK, "F_OK", 0, 3},
	{DNAME_RX"/"FNAME_W, R_OK, "R_OK", 0, 2},
	{DNAME_RX"/"FNAME_W, W_OK, "W_OK", 0, 3},
	{DNAME_RX"/"FNAME_X, F_OK, "F_OK", 0, 3},
	{DNAME_RX"/"FNAME_X, R_OK, "R_OK", 0, 2},
	{DNAME_RX"/"FNAME_X, W_OK, "W_OK", 0, 2},
	{DNAME_RX"/"FNAME_X, X_OK, "X_OK", 0, 3},
	{DNAME_WX"/"FNAME_R, F_OK, "F_OK", 0, 3},
	{DNAME_WX"/"FNAME_R, R_OK, "R_OK", 0, 3},
	{DNAME_WX"/"FNAME_R, W_OK, "W_OK", 0, 2},
	{DNAME_WX"/"FNAME_W, F_OK, "F_OK", 0, 3},
	{DNAME_WX"/"FNAME_W, R_OK, "R_OK", 0, 2},
	{DNAME_WX"/"FNAME_W, W_OK, "W_OK", 0, 3},
	{DNAME_WX"/"FNAME_X, F_OK, "F_OK", 0, 3},
	{DNAME_WX"/"FNAME_X, R_OK, "R_OK", 0, 2},
	{DNAME_WX"/"FNAME_X, W_OK, "W_OK", 0, 2},
	{DNAME_WX"/"FNAME_X, X_OK, "X_OK", 0, 3},
	{DNAME_R"/"FNAME_R, F_OK, "F_OK", EACCES, 1},
	{DNAME_R"/"FNAME_R, R_OK, "R_OK", EACCES, 1},
	{DNAME_R"/"FNAME_R, W_OK, "W_OK", EACCES, 1},
	{DNAME_R"/"FNAME_R, X_OK, "X_OK", EACCES, 3},
	{DNAME_R"/"FNAME_W, F_OK, "F_OK", EACCES, 1},
	{DNAME_R"/"FNAME_W, R_OK, "R_OK", EACCES, 1},
	{DNAME_R"/"FNAME_W, W_OK, "W_OK", EACCES, 1},
	{DNAME_R"/"FNAME_W, X_OK, "X_OK", EACCES, 3},
	{DNAME_R"/"FNAME_X, F_OK, "F_OK", EACCES, 1},
	{DNAME_R"/"FNAME_X, R_OK, "R_OK", EACCES, 1},
	{DNAME_R"/"FNAME_X, W_OK, "W_OK", EACCES, 1},
	{DNAME_R"/"FNAME_X, X_OK, "X_OK", EACCES, 1},
	{DNAME_W"/"FNAME_R, F_OK, "F_OK", EACCES, 1},
	{DNAME_W"/"FNAME_R, R_OK, "R_OK", EACCES, 1},
	{DNAME_W"/"FNAME_R, W_OK, "W_OK", EACCES, 1},
	{DNAME_W"/"FNAME_R, X_OK, "X_OK", EACCES, 3},
	{DNAME_W"/"FNAME_W, F_OK, "F_OK", EACCES, 1},
	{DNAME_W"/"FNAME_W, R_OK, "R_OK", EACCES, 1},
	{DNAME_W"/"FNAME_W, W_OK, "W_OK", EACCES, 1},
	{DNAME_W"/"FNAME_W, X_OK, "X_OK", EACCES, 3},
	{DNAME_W"/"FNAME_X, F_OK, "F_OK", EACCES, 1},
	{DNAME_W"/"FNAME_X, R_OK, "R_OK", EACCES, 1},
	{DNAME_W"/"FNAME_X, W_OK, "W_OK", EACCES, 1},
	{DNAME_W"/"FNAME_X, X_OK, "X_OK", EACCES, 1},
	{DNAME_X"/"FNAME_R, W_OK, "W_OK", EACCES, 1},
	{DNAME_X"/"FNAME_R, X_OK, "X_OK", EACCES, 3},
	{DNAME_X"/"FNAME_W, R_OK, "R_OK", EACCES, 1},
	{DNAME_X"/"FNAME_W, X_OK, "X_OK", EACCES, 3},
	{DNAME_X"/"FNAME_X, R_OK, "R_OK", EACCES, 1},
	{DNAME_X"/"FNAME_X, W_OK, "W_OK", EACCES, 1},
	{DNAME_RW"/"FNAME_R, F_OK, "F_OK", EACCES, 1},
	{DNAME_RW"/"FNAME_R, R_OK, "R_OK", EACCES, 1},
	{DNAME_RW"/"FNAME_R, W_OK, "W_OK", EACCES, 1},
	{DNAME_RW"/"FNAME_R, X_OK, "X_OK", EACCES, 3},
	{DNAME_RW"/"FNAME_W, F_OK, "F_OK", EACCES, 1},
	{DNAME_RW"/"FNAME_W, R_OK, "R_OK", EACCES, 1},
	{DNAME_RW"/"FNAME_W, W_OK, "W_OK", EACCES, 1},
	{DNAME_RW"/"FNAME_W, X_OK, "X_OK", EACCES, 3},
	{DNAME_RW"/"FNAME_X, F_OK, "F_OK", EACCES, 1},
	{DNAME_RW"/"FNAME_X, R_OK, "R_OK", EACCES, 1},
	{DNAME_RW"/"FNAME_X, W_OK, "W_OK", EACCES, 1},
	{DNAME_RW"/"FNAME_X, X_OK, "X_OK", EACCES, 1},
	{DNAME_RX"/"FNAME_R, W_OK, "W_OK", EACCES, 1},
	{DNAME_RX"/"FNAME_R, X_OK, "X_OK", EACCES, 3},
	{DNAME_RX"/"FNAME_W, R_OK, "R_OK", EACCES, 1},
	{DNAME_RX"/"FNAME_W, X_OK, "X_OK", EACCES, 3},
	{DNAME_RX"/"FNAME_X, R_OK, "R_OK", EACCES, 1},
	{DNAME_RX"/"FNAME_X, W_OK, "W_OK", EACCES, 1},
	{DNAME_WX"/"FNAME_R, W_OK, "W_OK", EACCES, 1},
	{DNAME_WX"/"FNAME_R, X_OK, "X_OK", EACCES, 3},
	{DNAME_WX"/"FNAME_W, R_OK, "R_OK", EACCES, 1},
	{DNAME_WX"/"FNAME_W, X_OK, "X_OK", EACCES, 3},
	{DNAME_WX"/"FNAME_X, R_OK, "R_OK", EACCES, 1},
	{DNAME_WX"/"FNAME_X, W_OK, "W_OK", EACCES, 1}
};

int access_test(struct tcase *tc, const char *user)
{
	int ret;
	printf("access(%s, %s) as %s", tc->fname, tc->name, user);
	ret = access(tc->fname, tc->mode);
	if (tc->exp_errno) {
		if(ret || errno != tc->exp_errno)
			return 0;
		return 1;
	} else {
		if(ret == -1)
			return 0;
		return 1;
	}
}

int verify_access(unsigned int n)
{
	struct tcase *tc = tcases + n;
	pid_t pid;
	if (tc->exp_user & 0x02)
		return access_test(tc, "root");
	if (tc->exp_user & 0x01) {
		pid = fork();
		if (pid) {
			waitpid(pid, NULL, 0);
		} else {
			setuid(uid);
			return (tc, "nobody");
		}
	}
}

void setup(void)
{
	struct passwd *pw;
	umask(0022);
	pw = getpwnam("nobody");
	uid = pw->pw_uid;
	touch(FNAME_RWX, 0777, NULL);
	touch(FNAME_R, 0444, NULL);
	touch(FNAME_W, 0222, NULL);
	touch(FNAME_X, 0111, NULL);
	mkdir(DNAME_R, 0444);
	mkdir(DNAME_W, 0222);
	mkdir(DNAME_X, 0111);
	mkdir(DNAME_RW, 0666);
	mkdir(DNAME_RX, 0555);
	mkdir(DNAME_WX, 0333);
	touch(DNAME_R"/"FNAME_R, 0444, NULL);
	touch(DNAME_R"/"FNAME_W, 0222, NULL);
	touch(DNAME_R"/"FNAME_X, 0111, NULL);
	touch(DNAME_W"/"FNAME_R, 0444, NULL);
	touch(DNAME_W"/"FNAME_W, 0222, NULL);
	touch(DNAME_W"/"FNAME_X, 0111, NULL);
	touch(DNAME_X"/"FNAME_R, 0444, NULL);
	touch(DNAME_X"/"FNAME_W, 0222, NULL);
	touch(DNAME_X"/"FNAME_X, 0111, NULL);
	touch(DNAME_RW"/"FNAME_R, 0444, NULL);
	touch(DNAME_RW"/"FNAME_W, 0222, NULL);
	touch(DNAME_RW"/"FNAME_X, 0111, NULL);
	touch(DNAME_RX"/"FNAME_R, 0444, NULL);
	touch(DNAME_RX"/"FNAME_W, 0222, NULL);
	touch(DNAME_RX"/"FNAME_X, 0111, NULL);
	touch(DNAME_WX"/"FNAME_R, 0444, NULL);
	touch(DNAME_WX"/"FNAME_W, 0222, NULL);
	touch(DNAME_WX"/"FNAME_X, 0111, NULL);
}

void main(){
	setup();
	int ok = 1, N = sizeof(tcases) / sizeof(tcases[0]);
	for(int i = 0; i<N; i++)
		ok *= verify_access(i);
	if(ok)
		printf("test succeeded");
}
