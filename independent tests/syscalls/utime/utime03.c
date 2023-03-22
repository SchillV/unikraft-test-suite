#include "incl.h"
#define MNTPOINT	"mntpoint"
#define TEMP_FILE	MNTPOINT"/tmp_file"
#define FILE_MODE	0766

uid_t root_uid, user_uid;

void setup(void)
{
	struct passwd *pw;
	uid_t test_users[2];
	int fd;
	root_uid = getuid();
	pw = getpwnam("nobody");
	test_users[0] = pw->pw_uid;
	tst_get_uids(test_users, 1, 2);
	user_uid = test_users[1];
	fd = creat(TEMP_FILE, FILE_MODE);
	close(fd);
	chmod(TEMP_FILE, FILE_MODE);
	chown(TEMP_FILE, pw->pw_uid, pw->pw_gid);
}

void run(void)
{
	struct utimbuf utbuf;
	struct stat statbuf;
	time_t mintime, maxtime;
	utbuf.modtime = time(0) - 5;
	utbuf.actime = utbuf.modtime + 1;
	TST_EXP_PASS_SILENT(utime(TEMP_FILE, &utbuf));
	stat(TEMP_FILE, &statbuf);
	if (statbuf.st_atime != utbuf.actime ||
		statbuf.st_mtime != utbuf.modtime) {
		tst_res(TFAIL, "Could not set initial file times");
		return;
	}
	seteuid(user_uid);
	mintime = tst_get_fs_timestamp();
	TST_EXP_PASS(utime(TEMP_FILE, NULL));
	maxtime = tst_get_fs_timestamp();
	seteuid(root_uid);
	stat(TEMP_FILE, &statbuf);
	if (statbuf.st_atime < mintime || statbuf.st_atime > maxtime)
		tst_res(TFAIL, "utime() did not set expected atime, "
			"mintime: %ld, maxtime: %ld, st_atime: %ld",
			mintime, maxtime, statbuf.st_atime);
	if (statbuf.st_mtime < mintime || statbuf.st_mtime > maxtime)
		tst_res(TFAIL, "utime() did not set expected mtime, "
			"mintime: %ld, maxtime: %ld, st_mtime: %ld",
			mintime, maxtime, statbuf.st_mtime);
}

void main(){
	setup();
	cleanup();
}
