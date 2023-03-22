#include <sys/types.h>
#include <sys/stat.h>
#include <sys/syscall.h>
#include <fcntl.h>
#include <unistd.h>
#include <stdlib.h>
#include <errno.h>
#include <string.h>
#include <signal.h>
#include <stdio.h>
#include <limits.h>

#define _GNU_SOURCE
#define TEST_CASES 6
#ifndef AT_FDCWD
#define AT_FDCWD -100
#endif
void setup();
void cleanup();
char *TCID = "faccessat01";
int TST_TOTAL = TEST_CASES;

char pathname[256];
char testfile[256];
char testfile2[256];
char testfile3[256];
int fds[TEST_CASES];
char *filenames[TEST_CASES];
int expected_errno[TEST_CASES] = { 0, 0, ENOTDIR, EBADF, 0, 0 };

int myfaccessat(int dirfd, const char *filename, int mode)
{
	return syscall(__NR_faccessat, dirfd, filename, mode);
}
int main(int ac, char **av)
{
	int i;
	setup();
	for (i = 0; i < TST_TOTAL; i++) {
		int ret = myfaccessat(fds[i], filenames[i], R_OK);
		if (errno == expected_errno[i]) {
			printf("faccessat() returned the expected  errno %d\n",errno);
		} else {
			printf("faccessdat() Failed, errno=%d\n",errno);
		}
	}
	cleanup();
}
void setup(void)
{
	char abs_path[PATH_MAX];
	getcwd(abs_path, sizeof(abs_path));
	int p = getpid();
	sprintf(pathname, "faccessattestdir%d", p);
	sprintf(testfile, "faccessattestfile%d.txt", p);
	sprintf(testfile2, "%s/faccessattestfile%d.txt", abs_path, p);
	sprintf(testfile3, "faccessattestdir%d/faccessattestfile%d.txt", p, p);
	mkdir(pathname, 0700);
	fds[0] = open(pathname, __O_DIRECTORY);
	fds[1] = fds[4] = fds[0];
	fprintf(testfile, "%s", testfile);
	fprintf(testfile2, "%s", testfile2);
	fds[2] = open(testfile3, O_CREAT | O_RDWR, 0600);
	fds[3] = 100;
	fds[5] = AT_FDCWD;
	filenames[0] = filenames[2] = filenames[3] = filenames[4] = testfile;
	filenames[1] = testfile2;
	filenames[5] = testfile3;
}
void cleanup(void)
{
	if (fds[0] > 0)
		close(fds[0]);
	if (fds[2] > 0)
		close(fds[2]);
}

