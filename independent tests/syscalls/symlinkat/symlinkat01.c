#include "incl.h"
#define _GNU_SOURCE
#define MYRETCODE -999
#ifndef AT_FDCWD
#define AT_FDCWD -100
#endif
struct test_struct;

void setup();

void cleanup();

void setup_every_copy();

void mysymlinkat_test(struct test_struct *desc);
#define TEST_DIR1 "olddir"
#define TEST_DIR2 "newdir"
#define TEST_DIR3 "deldir"
#define TEST_FILE1 "oldfile"
#define TEST_FILE2 "newfile"
#define TEST_FIFO "fifo"

char dpathname[256] = "%s/" TEST_DIR2 "/" TEST_FILE1;

int olddirfd, newdirfd = -1, cwd_fd = AT_FDCWD, stdinfd = 0, crapfd =
    -1, deldirfd;
struct test_struct {
	const char *oldfn;
	int *newfd;
	const char *newfn;
	const char *referencefn1;
	const char *referencefn2;
	int expected_errno;
} test_desc[] = {
	{
	"../" TEST_DIR1 "/" TEST_FILE1, &newdirfd, TEST_FILE1,
		    TEST_DIR1 "/" TEST_FILE1, TEST_DIR2 "/" TEST_FILE1, 0},
	{
	"../" TEST_DIR1 "/" TEST_FILE1, &newdirfd, dpathname,
		    TEST_DIR1 "/" TEST_FILE1, TEST_DIR2 "/" TEST_FILE1, 0},
	{
	"../" TEST_DIR1 "/" TEST_FILE1, &cwd_fd,
		    TEST_DIR2 "/" TEST_FILE1, TEST_DIR1 "/" TEST_FILE1,
		    TEST_DIR2 "/" TEST_FILE1, 0},
	{
	"../" TEST_DIR1 "/" TEST_FILE1, &cwd_fd, dpathname,
		    TEST_DIR1 "/" TEST_FILE1, TEST_DIR2 "/" TEST_FILE1, 0},
	{
	"../" TEST_DIR1 "/" TEST_FILE1, &stdinfd,
		    TEST_DIR2 "/" TEST_FILE1, 0, 0, ENOTDIR},
	{
	"../" TEST_DIR1 "/" TEST_FILE1, &stdinfd, dpathname,
		    TEST_DIR1 "/" TEST_FILE1, TEST_DIR2 "/" TEST_FILE1, 0},
	{
	"../" TEST_DIR1 "/" TEST_FILE1, &crapfd,
		    TEST_DIR2 "/" TEST_FILE1, 0, 0, EBADF},
	{
	"../" TEST_DIR1 "/" TEST_FILE1, &crapfd, dpathname,
		    TEST_DIR1 "/" TEST_FILE1, TEST_DIR2 "/" TEST_FILE1, 0},
	{
	"../" TEST_DIR1 "/" TEST_FILE1, &deldirfd,
		    TEST_DIR2 "/" TEST_FILE1, 0, 0, ENOENT},
	{
	"../" TEST_DIR1 "/" TEST_FILE1, &deldirfd, dpathname,
		    TEST_DIR1 "/" TEST_FILE1, TEST_DIR2 "/" TEST_FILE1, 0},
};
char *TCID = "symlinkat01";
int TST_TOTAL = sizeof(test_desc) / sizeof(*test_desc);

int mysymlinkat(const char *oldfilename,
		       int newdirfd, const char *newfilename)
{
	return tst_syscall(__NR_symlinkat, oldfilename, newdirfd, newfilename);
}
int main(int ac, char **av)
{
	int lc;
	int i;
	tst_parse_opts(ac, av, NULL, NULL);
	setup();
	for (lc = 0; TEST_LOOPING(lc); lc++) {
		tst_count = 0;
		for (i = 0; i < TST_TOTAL; i++) {
			setup_every_copy();
			mysymlinkat_test(&test_desc[i]);
		}
	}
	cleanup();
	tst_exit();
}

void setup_every_copy(void)
{
	close(newdirfd);
	unlink(dpathname);
	rmdir(TEST_DIR2);
	mkdir(cleanup, TEST_DIR2, 0700);
	newdirfd = open(cleanup, TEST_DIR2, O_DIRECTORY);
}

void mysymlinkat_test(struct test_struct *desc)
{
	int fd;
mysymlinkat(desc->oldfn, *desc->newfd, desc->newfn);
	if (TEST_ERRNO == desc->expected_errno) {
		if (TEST_RETURN == 0 && desc->referencefn1 != NULL) {
			int tnum = rand(), vnum = ~tnum;
			fd = open(cleanup, desc->referencefn1, O_RDWR);
			write(cleanup, 1, fd, &tnum,
				sizeof(tnum));
			close(cleanup, fd);
			fd = open(cleanup, desc->referencefn2, O_RDONLY);
			read(cleanup, 1, fd, &vnum, sizeofcleanup, 1, fd, &vnum, sizeofvnum));
			close(cleanup, fd);
			if (tnum == vnum)
				tst_resm(TPASS, "Test passed");
			else
				tst_resm(TFAIL,
					 "The link file's content isn't as same as the original file's "
					 "although symlinkat returned 0");
		} else {
			tst_resm(TPASS,
				 "symlinkat() returned the expected  errno %d: %s",
				 TEST_ERRNO, strerror(TEST_ERRNO));
		}
	} else {
		tst_resm(TFAIL,
			 TEST_RETURN ==
			 0 ? "symlinkat() surprisingly succeeded" :
			 "symlinkat() Failed, errno=%d : %s", TEST_ERRNO,
			 strerror(TEST_ERRNO));
	}
}

void setup(void)
{
	char *tmp;
	int fd;
	tst_sig(NOFORK, DEF_HANDLER, cleanup);
	tst_tmpdir();
	mkdir(cleanup, TEST_DIR1, 0700);
	mkdir(cleanup, TEST_DIR3, 0700);
	olddirfd = open(cleanup, TEST_DIR1, O_DIRECTORY);
	deldirfd = open(cleanup, TEST_DIR3, O_DIRECTORY);
	rmdir(cleanup, TEST_DIR3);
	fd = open(cleanup, TEST_DIR1 "/" TEST_FILE1, O_CREAT | O_EXCL, 0600);
	close(cleanup, fd);
	tmp = strdup(dpathname);
	snprintf(dpathname, sizeof(dpathname), tmp, get_current_dir_name());
	TEST_PAUSE;
}

void cleanup(void)
{
	tst_rmdir();
}

