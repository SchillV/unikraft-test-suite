#include "incl.h"
#define _GNU_SOURCE
#ifndef AT_FDCWD
#define AT_FDCWD -100
#endif
#ifndef AT_SYMLINK_FOLLOW
#define AT_SYMLINK_FOLLOW 0x400
#endif
struct test_struct;

void setup();

void cleanup();

void setup_every_copy();

void mylinkat_test(struct test_struct *desc);
#define TEST_DIR1 "olddir"
#define TEST_DIR2 "newdir"
#define TEST_DIR3 "deldir"
#define TEST_FILE1 "oldfile"
#define TEST_FILE2 "newfile"
#define TEST_FIFO "fifo"
#define DPATHNAME_FMT	"%s/" TEST_DIR2 "/" TEST_FILE1
#define SPATHNAME_FMT	"%s/" TEST_DIR1 "/" TEST_FILE1

char dpathname[PATH_MAX];

char spathname[PATH_MAX];

int olddirfd, newdirfd = -1, cwd_fd = AT_FDCWD, stdinfd = 0, badfd =
    -1, deldirfd;
struct test_struct {
	int *oldfd;
	const char *oldfn;
	int *newfd;
	const char *newfn;
	int flags;
	const char *referencefn1;
	const char *referencefn2;
	int expected_errno;
} test_desc[] = {
	{
	&olddirfd, TEST_FILE1, &newdirfd, TEST_FILE1, 0,
		    TEST_DIR1 "/" TEST_FILE1, TEST_DIR2 "/" TEST_FILE1, 0},
	{
	&olddirfd, spathname, &newdirfd, TEST_FILE1, 0, 0, 0, 0},
	{
	&olddirfd, TEST_FILE1, &newdirfd, dpathname, 0,
		    TEST_DIR1 "/" TEST_FILE1, TEST_DIR2 "/" TEST_FILE1, 0},
	{
	&cwd_fd, TEST_DIR1 "/" TEST_FILE1, &newdirfd, TEST_FILE1, 0,
		    TEST_DIR1 "/" TEST_FILE1, TEST_DIR2 "/" TEST_FILE1, 0},
	{
	&olddirfd, TEST_FILE1, &cwd_fd, TEST_DIR2 "/" TEST_FILE1, 0,
		    TEST_DIR1 "/" TEST_FILE1, TEST_DIR2 "/" TEST_FILE1, 0},
	{
	&cwd_fd, spathname, &newdirfd, TEST_FILE1, 0, 0, 0, 0},
	{
	&olddirfd, TEST_FILE1, &cwd_fd, dpathname, 0,
		    TEST_DIR1 "/" TEST_FILE1, TEST_DIR2 "/" TEST_FILE1, 0},
	{
	&stdinfd, TEST_DIR1 "/" TEST_FILE1, &newdirfd, TEST_FILE1, 0,
		    0, 0, ENOTDIR},
	{
	&olddirfd, TEST_FILE1, &stdinfd, TEST_DIR2 "/" TEST_FILE1, 0,
		    0, 0, ENOTDIR},
	{
	&stdinfd, spathname, &newdirfd, TEST_FILE1, 0, 0, 0, 0},
	{
	&olddirfd, TEST_FILE1, &stdinfd, dpathname, 0,
		    TEST_DIR1 "/" TEST_FILE1, TEST_DIR2 "/" TEST_FILE1, 0},
	{
	&badfd, TEST_DIR1 "/" TEST_FILE1, &newdirfd, TEST_FILE1, 0,
		    0, 0, EBADF},
	{
	&olddirfd, TEST_FILE1, &badfd, TEST_DIR2 "/" TEST_FILE1, 0,
		    0, 0, EBADF},
	{
	&badfd, spathname, &newdirfd, TEST_FILE1, 0, 0, 0, 0},
	{
	&olddirfd, TEST_FILE1, &badfd, dpathname, 0,
		    TEST_DIR1 "/" TEST_FILE1, TEST_DIR2 "/" TEST_FILE1, 0},
	{
	&deldirfd, TEST_DIR1 "/" TEST_FILE1, &newdirfd, TEST_FILE1, 0,
		    0, 0, ENOENT},
	{
	&olddirfd, TEST_FILE1, &deldirfd, TEST_DIR2 "/" TEST_FILE1, 0,
		    0, 0, ENOENT},
	{
	&deldirfd, spathname, &newdirfd, TEST_FILE1, 0, 0, 0, 0},
	{
	&olddirfd, TEST_FILE1, &deldirfd, dpathname, 0,
		    TEST_DIR1 "/" TEST_FILE1, TEST_DIR2 "/" TEST_FILE1, 0},
	{
	&cwd_fd, "/proc/cpuinfo", &newdirfd, TEST_FILE1, 0, 0, 0, EXDEV},
	{
	&olddirfd, ".", &newdirfd, TEST_FILE1, 0, 0, 0, EPERM},
	{
	&olddirfd, TEST_FILE1, &newdirfd, TEST_FILE1, 1, 0, 0, EINVAL},
	     * find a legitimate means to exercise this functionality, if in fact
	     * it's a valid testcase -- which it should be.
	     */
	       TEST_DIR1"/"TEST_FIFO, TEST_DIR2"/"TEST_FILE1, 0 } */
};
char *TCID = "linkat01";
int TST_TOTAL = sizeof(test_desc) / sizeof(*test_desc);

int mylinkat(int olddirfd, const char *oldfilename, int newdirfd,
		    const char *newfilename, int flags)
{
	return tst_syscall(__NR_linkat, olddirfd, oldfilename, newdirfd,
		       newfilename, flags);
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
			mylinkat_test(&test_desc[i]);
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

void mylinkat_test(struct test_struct *desc)
{
	int fd;
mylink;
	     (*desc->oldfd, desc->oldfn, *desc->newfd, desc->newfn,
	      desc->flags));
	if (TEST_ERRNO == desc->expected_errno) {
		if (TEST_RETURN == 0 && desc->referencefn1 != NULL) {
			int tnum = rand(), vnum = ~tnum;
			fd = open(cleanup, desc->referencefn1,
				       O_RDWR);
			write(cleanup, 1, fd, &tnum,
				sizeof(tnum));
			close(cleanup, fd);
			fd = open(cleanup, desc->referencefn2,
				       O_RDONLY);
			read(cleanup, 1, fd, &vnum, sizeofcleanup, 1, fd, &vnum, sizeofvnum));
			close(cleanup, fd);
			if (tnum == vnum)
				tst_resm(TPASS,
					 "linkat is functionality correct");
			else {
				tst_resm(TFAIL,
					 "The link file's content isn't "
					 "as same as the original file's "
					 "although linkat returned 0");
			}
		} else {
			if (TEST_RETURN == 0)
				tst_resm(TPASS,
					 "linkat succeeded as expected");
			else
				tst_resm(TPASS | TTERRNO,
					 "linkat failed as expected");
		}
	} else {
		if (TEST_RETURN == 0)
			tst_resm(TFAIL, "linkat succeeded unexpectedly");
		else
			tst_resm(TFAIL | TTERRNO,
				 "linkat failed unexpectedly; expected %d - %s",
				 desc->expected_errno,
				 strerror(desc->expected_errno));
	}
}
void setup(void)
{
	char *cwd;
	int fd;
	tst_sig(NOFORK, DEF_HANDLER, cleanup);
	tst_tmpdir();
	cwd = get_current_dir_name();
	if (cwd == NULL) {
		tst_brkm(TFAIL | TERRNO, cleanup,
			 "Failed to get current working directory");
	}
	mkdir(cleanup, TEST_DIR1, 0700);
	mkdir(cleanup, TEST_DIR3, 0700);
	olddirfd = open(cleanup, TEST_DIR1, O_DIRECTORY);
	deldirfd = open(cleanup, TEST_DIR3, O_DIRECTORY);
	rmdir(cleanup, TEST_DIR3);
	fd = open(cleanup, TEST_DIR1 "/" TEST_FILE1, O_CREAT | O_EXCL, 0600);
	close(cleanup, fd);
	mkfifo(cleanup, TEST_DIR1 "/" TEST_FIFO, 0600);
	snprintf(dpathname, sizeof(dpathname), DPATHNAME_FMT, cwd);
	snprintf(spathname, sizeof(spathname), SPATHNAME_FMT, cwd);
	free(cwd);
}

void cleanup(void)
{
	tst_rmdir();
}

