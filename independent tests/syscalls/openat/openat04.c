#include "incl.h"
#define _GNU_SOURCE
#define MODE_RWX        0777
#define MODE_SGID       (S_ISGID|0777)
#define MNTPOINT	"mntpoint"
#define WORKDIR		MNTPOINT "/testdir"
#define OPEN_FILE	"open.tmp"

gid_t free_gid;

int tmpfile_fd = -1, dir_fd = -1, mount_flag;

struct passwd *ltpuser;

void do_mount(const char *source, const char *target,
	const char *filesystemtype, unsigned long mountflags,
	const void *data)
{
	int ret = mount(source, target, filesystemtype, mountflags, data);
	if (ret == -1 && errno == EINVAL){
		printf("Kernel does not support noacl feature\n");
		exit(0);
	} if (ret == -1) {
		printf("mount(%s, %s, %s, %lu, %p) failed, error number %d\n", source, target, filesystemtype, mountflags, data, errno);
		exit(0);
	}
	if (ret){
		printf("Invalid mount return value %ld\n", ret);
		exit(0);
	}
	mount_flag = 1;
}

void open_tmpfile_supported(int dirfd)
{
	int ret = openat(dirfd, ".", __O_TMPFILE | O_RDWR, S_IXGRP | S_ISGID);
	if (ret == -1) {
		if (errno == ENOTSUP)
			printf("fs doesn't support O_TMPFILE\n");
		else
			printf("openat(%d, O_TMPFILE) failed, error number %d\n", dirfd, errno);
		exit(0);
	}
	if (ret < 0){
		printf("Invalid openat return value %ld\n", ret);
		exit(0);
	}
	close(ret);
}

void setup(void)
{
	struct stat buf;
	ltpuser = getpwnam("nobody");
	printf("[I] User nobody: uid = %d, gid = %d\n", (int)ltpuser->pw_uid,
		(int)ltpuser->pw_gid);
	mkdir(WORKDIR, MODE_RWX);
	dir_fd = open(WORKDIR, O_RDONLY, __O_DIRECTORY);
	open_tmpfile_supported(dir_fd);
	chown(WORKDIR, ltpuser->pw_uid, free_gid);
	chmod(WORKDIR, MODE_SGID);
	stat(WORKDIR, &buf);
	if (!(buf.st_mode & S_ISGID)){
		printf("%s: Setgid bit not set\n", WORKDIR);
		exit(0);
	}
	if (buf.st_gid != free_gid) {
		printf("%s: Incorrect group, %u != %u\n", WORKDIR, buf.st_gid, free_gid);
		exit(0);
	}
	setgid(ltpuser->pw_gid);
	setreuid(-1, ltpuser->pw_uid);
}

int file_test(int dfd, const char *path, int flags)
{
	struct stat buf;
	fstatat(dfd, path, &buf, flags);
	if (buf.st_mode & S_ISGID) {
		printf("%s: Setgid bit is set\n", path);
		return 0;
	} else {
		printf("%s: Setgid bit not set\n", path);
	} if (buf.st_mode & S_IXGRP) {
		printf("%s: S_IXGRP bit is set\n", path);
		return 0;
	} else {
		printf("%s: S_IXGRP bit is not set\n", path);
		return 1;
	}
}

int run(void)
{
	char path[PATH_MAX];
	umask(S_IXGRP);
	tmpfile_fd = openat(dir_fd, ".", __O_TMPFILE | O_RDWR, MODE_SGID);
	snprintf(path, PATH_MAX, "/proc/self/fd/%d", tmpfile_fd);
	linkat(AT_FDCWD, path, dir_fd, OPEN_FILE, AT_SYMLINK_FOLLOW);
	return file_test(dir_fd, OPEN_FILE, 0);
}

void cleanup(void)
{
	setreuid(-1, 0);
	if (tmpfile_fd >= 0)
		close(tmpfile_fd);
	if (dir_fd >= 0)
		close(dir_fd);
}

void main(){
	setup();
	if(run())
		printf("test succeeded\n");
	cleanup();
}
