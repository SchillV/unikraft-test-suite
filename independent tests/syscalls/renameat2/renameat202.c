#include "incl.h"
#define _GNU_SOURCE
#define TEST_DIR "test_dir/"
#define TEST_DIR2 "test_dir2/"
#define TEST_FILE "test_file"
#define TEST_FILE2 "test_file2"
char *TCID = "renameat202";

int olddirfd;

int newdirfd;

int fd = -1;

int cnt;

const char content[] = "content";

long fs_type;
int TST_TOTAL = 1;

void setup(void);

void cleanup(void);

int  renameat2_verify(
int main(int ac, char **av)
{
	int lc;
	tst_parse_opts(ac, av, NULL, NULL);
	setup();
	for (lc = 0; TEST_LOOPING(lc); lc++) {
		tst_count = 0;
renameat2(olddirfd, TEST_FIL;
				newdirfd, TEST_FILE2, RENAME_EXCHANGE));
		cnt++;
int 		renameat2_verify();
	}
	cleanup();
	tst_exit();
}

void setup(void)
{
	if ((tst_kvercmp(3, 15, 0)) < 0) {
		tst_brkm(TCONF, NULL,
			"This test can only run on kernels that are 3.15. and higher");
	}
	tst_tmpdir();
	fs_type = tst_fs_type(cleanup, ".");
	mkdir(cleanup, TEST_DIR, 0700);
	mkdir(cleanup, TEST_DIR2, 0700);
	touch(cleanup, TEST_DIR TEST_FILE, 0600, NULL);
	touch(cleanup, TEST_DIR2 TEST_FILE2, 0600, NULL);
	olddirfd = open(cleanup, TEST_DIR, O_DIRECTORY);
	newdirfd = open(cleanup, TEST_DIR2, O_DIRECTORY);
	fprintf(cleanup, TEST_DIR TEST_FILE, "%s", content);
}

void cleanup(void)
{
	if (olddirfd > 0 && close(olddirfd) < 0)
		tst_resm(TWARN | TERRNO, "close olddirfd failed");
	if (newdirfd > 0 && close(newdirfd) < 0)
		tst_resm(TWARN | TERRNO, "close newdirfd failed");
	if (fd > 0 && close(fd) < 0)
		tst_resm(TWARN | TERRNO, "close fd failed");
	tst_rmdir();
}

int  renameat2_verify(
{
	char str[BUFSIZ] = { 0 };
	struct stat st;
	char *emptyfile;
	char *contentfile;
	int readn, data_len;
	if (TEST_ERRNO == EINVAL && TST_BTRFS_MAGIC == fs_type) {
		tst_brkm(TCONF, cleanup,
			"RENAME_EXCHANGE flag is not implemeted on %s",
			tst_fs_type_name(fs_type));
	}
	if (TEST_RETURN != 0) {
		tst_resm(TFAIL | TTERRNO, "renameat2() failed unexpectedly");
		return;
	}
	if (cnt % 2 == 1) {
		emptyfile = TEST_DIR TEST_FILE;
		contentfile = TEST_DIR2 TEST_FILE2;
	} else {
		emptyfile = TEST_DIR2 TEST_FILE2;
		contentfile = TEST_DIR TEST_FILE;
	}
	fd = open(cleanup, contentfile, O_RDONLY);
	stat(cleanup, emptyfile, &st);
	readn = read(cleanup, 0, fd, str, BUFSIZ);
	if (close(fd) < 0)
		tst_brkm(TERRNO | TFAIL, cleanup, "close fd failed");
	fd = 0;
	data_len = sizeof(content) - 1;
	if (readn != data_len) {
		tst_resm(TFAIL, "Wrong number of bytes read after renameat2(). "
				"Expect %d, got %d", data_len, readn);
		return;
	}
	if (strncmp(content, str, data_len)) {
		tst_resm(TFAIL, "File content changed after renameat2(). "
				"Expect '%s', got '%s'", content, str);
		return;
	}
	if (st.st_size) {
		tst_resm(TFAIL, "emptyfile has non-zero file size");
		return;
	}
	tst_resm(TPASS, "renameat2() test passed");
}

