#include "incl.h"
  * Copyright (c) International Business Machines  Corp., 2002
  * Copyright (c) 2015 Cyril Hrubis <chrubis@suse.cz>
  * Copyright (c) 2019 FUJITSU LIMITED. All rights reserved.
  *
  * Robbie Williamson <robbiew@us.ibm.com>
  * Roy Lee <roylee@andestech.com>
  */
#define RECLEN	100
#define MNTPOINT	"mntpoint"
#define TESTFILE	MNTPOINT"/testfile"

int len = 8 * 1024;

int recstart, reclen;

void ftruncate_expect_fail(int fd, off_t offset, const char *msg)
{
ftruncate(fd, offset);
	if (TST_RET == 0) {
		tst_res(TFAIL, "ftruncate() %s succeeded unexpectedly", msg);
		return;
	}
	if (TST_ERR != EAGAIN) {
		tst_res(TFAIL | TTERRNO,
			"ftruncate() %s failed unexpectedly, expected EAGAIN",
			msg);
		return;
	}
	tst_res(TPASS, "ftruncate() %s failed with EAGAIN", msg);
}

void ftruncate_expect_success(int fd, off_t offset, const char *msg)
{
	struct stat sb;
ftruncate(fd, offset);
	if (TST_RET != 0) {
		tst_res(TFAIL | TTERRNO,
			"ftruncate() %s failed unexpectedly", msg);
		return;
	}
	fstat(fd, &sb);
	if (sb.st_size != offset) {
		tst_res(TFAIL,
			"ftruncate() to %li bytes succeded but fstat() reports size %li",
			(long)offset, (long)sb.st_size);
		return;
	}
	tst_res(TPASS, "ftruncate() %s succeded", msg);
}

void doparent(void)
{
	int fd;
	TST_CHECKPOINT_WAIT(0);
	fd = open(TESTFILE, O_RDWR | O_NONBLOCK);
	ftruncate_expect_fail(fd, RECLEN, "offset before lock");
	ftruncate_expect_fail(fd, recstart + RECLEN/2, "offset in lock");
	ftruncate_expect_success(fd, recstart + RECLEN, "offset after lock");
	TST_CHECKPOINT_WAKE(0);
	wait(NULL);
	ftruncate_expect_success(fd, recstart + RECLEN/2, "offset in lock");
	ftruncate_expect_success(fd, recstart, "offset before lock");
	ftruncate_expect_success(fd, recstart + RECLEN, "offset after lock");
	close(fd);
}
void dochild(void)
{
	int fd;
	struct flock flocks;
	fd = open(TESTFILE, O_RDWR);
	tst_res(TINFO, "Child locks file");
	flocks.l_type = F_WRLCK;
	flocks.l_whence = SEEK_CUR;
	flocks.l_start = recstart;
	flocks.l_len = reclen;
	fcntl(fd, F_SETLKW, &flocks);
	TST_CHECKPOINT_WAKE_AND_WAIT(0);
	tst_res(TINFO, "Child unlocks file");
	exit(0);
}

int  verify_ftruncate(
{
	int pid;
	if (tst_fill_file(TESTFILE, 0, 1024, 8))
		tst_brk(TBROK, "Failed to create test file");
	chmod(TESTFILE, 02666);
	reclen = RECLEN;
	recstart = RECLEN + rand() % (len - 3 * RECLEN);
	pid = fork();
	if (pid == 0)
		dochild();
	doparent();
}

void setup(void)
{
	  * Kernel returns EPERM when CONFIG_MANDATORY_FILE_LOCKING is not
	  * supported - to avoid false negatives, mount the fs first without
	  * flags and then remount it as MS_MANDLOCK
	  */
	if (mount(NULL, MNTPOINT, NULL, MS_REMOUNT|MS_MANDLOCK, NULL) == -1) {
		if (errno == EPERM) {
			tst_brk(TCONF,
				"Mandatory lock not supported by this system");
		} else {
			tst_brk(TBROK | TTERRNO,
				"Remount with MS_MANDLOCK failed");
		}
	}
}

void main(){
	setup();
	cleanup();
}
