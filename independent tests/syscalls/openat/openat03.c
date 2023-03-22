#include "incl.h"
#define _GNU_SOURCE
char *TCID = "openat03";
int TST_TOTAL = 3;

ssize_t size;

char buf[1024];

const ssize_t blocks_num = 4;

struct stat st;

void cleanup(void)
{
}

void setup(void)
{
	size = sizeof(buf);
	memset(buf, 1, size);
	int fd = openat(AT_FDCWD, ".", __O_TMPFILE | O_RDWR, 0600);
	if (fd == -1) {
		if (errno == EISDIR || errno == ENOTSUP){
			printf("O_TMPFILE not supported\n");
			cleanup();
		}
		printf("openat() failed, error number %d\n", errno);
		cleanup();
	}
	close(fd);
}

int openat_tmp(int mode)
{
	int fd = openat(AT_FDCWD, ".", __O_TMPFILE | O_RDWR, mode);
	if (fd >= 0)
		return fd;
	printf("openat() failed, error number %d\n", errno);
	cleanup();
}

void write_file(int fd)
{
	int i;
	for (i = 0; i < blocks_num; ++i)
		write(fd, buf, size);
}
int test01(void)
{
	int fd;
	char path[PATH_MAX], tmp[PATH_MAX];
	printf("[I] creating a file with O_TMPFILE flag\n");
	fd = openat_tmp(0600);
	printf("[I] writing data to the file\n");
	write_file(fd);
	fstat(fd, &st);
	printf("file size is '%li'\n", (long)st.st_size);
	if (st.st_size != blocks_num * size) {
		printf("not expected size: '%li' != '%zu'\n",
			 (long)st.st_size, blocks_num * size);
		close(fd);
		return 0;
	}
	printf("[I] looking for the file in '.'\n");
	if ((st.st_mode & __S_IFDIR)){
		printf("found a file, this is not expected\n");
		cleanup();
		return 0;
	}
	printf("[I] file not found, OK\n");
	snprintf(path, PATH_MAX,  "/proc/self/fd/%d", fd);
	readlink(path, tmp, PATH_MAX);
	printf("renaming '%s' -> 'tmpfile'\n", tmp);
	linkat(AT_FDCWD, path, AT_FDCWD, "tmpfile", AT_SYMLINK_FOLLOW);
	if (st.st_mode & __S_IFDIR){
		printf("file not found\n");
		cleanup();
		return 0;
	}
	unlink("tmpfile");
	close(fd);
	printf("single file tests passed\n");
	return 1;
}

int read_file(int fd)
{
	int i;
	char tmp[size];
	lseek(fd, 0, SEEK_SET);
	for (i = 0; i < blocks_num; ++i) {
		read(fd, tmp, size);
		if (memcmp(buf, tmp, size)){
			printf("got unexepected data\n");
			cleanup();
			return 0;
		}
	}
	return 1;
}

int test02(void)
{
	const int files_num = 100;
	int i, fd[files_num];
	char path[PATH_MAX];
	printf("[I] create files in multiple directories\n");
	for (i = 0; i < files_num; ++i) {
		snprintf(path, PATH_MAX, "tst02_%d", i);
		mkdir(path, 0700);
		chdir(path);
		fd[i] = openat_tmp(0600);
	}
	printf("[I] removing test directories\n");
	for (i = files_num - 1; i >= 0; --i) {
		chdir("..");
		snprintf(path, PATH_MAX, "tst02_%d", i);
		rmdir(path);
	}
	printf("[I] writing/reading temporary files\n");
	for (i = 0; i < files_num; ++i) {
		write_file(fd[i]);
		if(!read_file(fd[i]))
			return 0;
	}
	printf("[I] closing temporary files\n");
	for (i = 0; i < files_num; ++i)
		close(fd[i]);
	printf("multiple files tests passed\n");
	return 1;
}

void link_tmp_file(int fd)
{
	char path1[PATH_MAX], path2[PATH_MAX];
	snprintf(path1, PATH_MAX,  "/proc/self/fd/%d", fd);
	snprintf(path2, PATH_MAX,  "tmpfile_%d", fd);
	linkat(AT_FDCWD, path1, AT_FDCWD, path2, AT_SYMLINK_FOLLOW);
}

int test03(void)
{
	const int files_num = 100;
	const mode_t test_perms[] = { 0, 07777, 001, 0755, 0644, 0440 };
	int i, fd[files_num];
	char path[PATH_MAX];
	struct stat st;
	mode_t mask = umask(0), perm;
	umask(mask);
	printf("[I] create multiple directories, link files into them\n");
	printf("[I] and check file permissions\n");
	for (i = 0; i < files_num; ++i) {
		snprintf(path, PATH_MAX, "tst03_%d", i);
		mkdir(path, 0700);
		chdir(path);
		perm = test_perms[i % (sizeof(test_perms)/sizeof(test_perms[0]))];
		fd[i] = openat_tmp(perm);
		write_file(fd[i]);
		read_file(fd[i]);
		link_tmp_file(fd[i]);
		snprintf(path, PATH_MAX, "tmpfile_%d", fd[i]);
		lstat(path, &st);
		mode_t exp_mode = perm & ~mask;
		if ((st.st_mode & ~__S_IFMT) != exp_mode) {
			printf("file mode read %o, but expected %o\n", st.st_mode & ~__S_IFMT, exp_mode);
			return 0;
		}
	}
	printf("[I] remove files, directories\n");
	for (i = files_num - 1; i >= 0; --i) {
		snprintf(path, PATH_MAX, "tmpfile_%d", fd[i]);
		unlink(path);
		close(fd[i]);
		chdir("..");
		snprintf(path, PATH_MAX, "tst03_%d", i);
		rmdir(path);
	}
	printf("file permission tests passed\n");
	return 1;
}
int main(int ac, char *av[])
{
	setup();
	if(test01() &&  test02() && test03())
		printf("test succeeded\n");
	cleanup();
}

