#include "incl.h"
#define _GNU_SOURCE
char *TCID = "open14";
int TST_TOTAL = 3;

ssize_t size;

char buf[1024];

const ssize_t blocks_num = 4;

struct stat st;

void cleanup(void)
{
	tst_rmdir();
}

void setup(void)
{
	tst_tmpdir();
	size = sizeof(buf);
	memset(buf, 1, size);
	int fd = open(".", O_TMPFILE | O_RDWR, 0600);
	if (fd == -1) {
		if (errno == EISDIR || errno == ENOTSUP)
			tst_brkm(TCONF, cleanup, "O_TMPFILE not supported");
		tst_brkm(TBROK | TERRNO, cleanup, "open() failed");
	}
	close(cleanup, fd);
}

void write_file(int fd)
{
	int i;
	for (i = 0; i < blocks_num; ++i)
		write(cleanup, 1, fd, buf, size);
}
void test01(void)
{
	int fd;
	char path[PATH_MAX], tmp[PATH_MAX];
	tst_resm(TINFO, "creating a file with O_TMPFILE flag");
	fd = open(cleanup, ".", O_TMPFILE | O_RDWR, 0600);
	tst_resm(TINFO, "writing data to the file");
	write_file(fd);
	fstat(cleanup, fd, &st);
	tst_resm(TINFO, "file size is '%li'", (long)st.st_size);
	if (st.st_size != blocks_num * size) {
		tst_resm(TFAIL, "not expected size: '%li' != '%zu'",
			 (long)st.st_size, blocks_num * size);
		close(cleanup, fd);
		return;
	}
	tst_resm(TINFO, "looking for the file in '.'");
	if (!tst_dir_is_empty(cleanup, ".", 1))
		tst_brkm(TFAIL, cleanup, "found a file, this is not expected");
	tst_resm(TINFO, "file not found, OK");
	snprintf(path, PATH_MAX,  "/proc/self/fd/%d", fd);
	readlink(cleanup, path, tmp, PATH_MAX);
	tst_resm(TINFO, "renaming '%s' -> 'tmpfile'", tmp);
	linkat(cleanup, AT_FDCWD, path, AT_FDCWD, "tmpfile",
		    AT_SYMLINK_FOLLOW);
	if (tst_dir_is_empty(cleanup, ".", 1))
		tst_brkm(TFAIL, cleanup, "file not found");
	unlink(cleanup, "tmpfile");
	close(cleanup, fd);
	tst_resm(TPASS, "single file tests passed");
}

void read_file(int fd)
{
	int i;
	char tmp[size];
	lseek(cleanup, fd, 0, SEEK_SET);
	for (i = 0; i < blocks_num; ++i) {
		read(cleanup, 0, fd, tmp, size);
		if (memcmp(buf, tmp, size))
			tst_brkm(TFAIL, cleanup, "got unexepected data");
	}
}

void test02(void)
{
	const int files_num = 100;
	int i, fd[files_num];
	char path[PATH_MAX];
	tst_resm(TINFO, "create files in multiple directories");
	for (i = 0; i < files_num; ++i) {
		snprintf(path, PATH_MAX, "tst02_%d", i);
		mkdir(cleanup, path, 0700);
		chdir(cleanup, path);
		fd[i] = open(cleanup, ".", O_TMPFILE | O_RDWR, 0600);
	}
	tst_resm(TINFO, "removing test directories");
	for (i = files_num - 1; i >= 0; --i) {
		chdir(cleanup, "..");
		snprintf(path, PATH_MAX, "tst02_%d", i);
		rmdir(cleanup, path);
	}
	tst_resm(TINFO, "writing/reading temporary files");
	for (i = 0; i < files_num; ++i) {
		write_file(fd[i]);
		read_file(fd[i]);
	}
	tst_resm(TINFO, "closing temporary files");
	for (i = 0; i < files_num; ++i)
		close(cleanup, fd[i]);
	tst_resm(TPASS, "multiple files tests passed");
}

void link_tmp_file(int fd)
{
	char path1[PATH_MAX], path2[PATH_MAX];
	snprintf(path1, PATH_MAX,  "/proc/self/fd/%d", fd);
	snprintf(path2, PATH_MAX,  "tmpfile_%d", fd);
	linkat(cleanup, AT_FDCWD, path1, AT_FDCWD, path2,
		    AT_SYMLINK_FOLLOW);
}

void test03(void)
{
	const int files_num = 100;
	const mode_t test_perms[] = { 0, 07777, 001, 0755, 0644, 0440 };
	int i, fd[files_num];
	char path[PATH_MAX];
	struct stat st;
	mode_t mask = umask(0), perm;
	umask(mask);
	tst_resm(TINFO, "create multiple directories, link files into them");
	tst_resm(TINFO, "and check file permissions");
	for (i = 0; i < files_num; ++i) {
		snprintf(path, PATH_MAX, "tst03_%d", i);
		mkdir(cleanup, path, 0700);
		chdir(cleanup, path);
		perm = test_perms[i % ARRAY_SIZE(test_perms)];
		fd[i] = open(cleanup, ".", O_TMPFILE | O_RDWR, perm);
		write_file(fd[i]);
		read_file(fd[i]);
		link_tmp_file(fd[i]);
		snprintf(path, PATH_MAX, "tmpfile_%d", fd[i]);
		lstat(cleanup, path, &st);
		mode_t exp_mode = perm & ~mask;
		if ((st.st_mode & ~S_IFMT) != exp_mode) {
			tst_brkm(TFAIL, cleanup,
				"file mode read %o, but expected %o",
				st.st_mode & ~S_IFMT, exp_mode);
		}
	}
	tst_resm(TINFO, "remove files, directories");
	for (i = files_num - 1; i >= 0; --i) {
		snprintf(path, PATH_MAX, "tmpfile_%d", fd[i]);
		unlink(cleanup, path);
		close(cleanup, fd[i]);
		chdir(cleanup, "..");
		snprintf(path, PATH_MAX, "tst03_%d", i);
		rmdir(cleanup, path);
	}
	tst_resm(TPASS, "file permission tests passed");
}
int main(int ac, char *av[])
{
	int lc;
	tst_parse_opts(ac, av, NULL, NULL);
	setup();
	for (lc = 0; TEST_LOOPING(lc); ++lc) {
		tst_count = 0;
		test01();
		test02();
		test03();
	}
	cleanup();
	tst_exit();
}

