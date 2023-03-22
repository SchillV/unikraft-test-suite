#include "incl.h"
#define FILE_MODE (S_IFREG|S_IRUSR|S_IWUSR|S_IRGRP|S_IROTH)
#define TESTFILE "testfile"

int fd;
struct test_case_t {
	char *desc;
	uid_t uid;
	gid_t gid;
} tc[] = {
	{"change owner/group ids", 700, 701},
	{"change owner id only", 702, -1},
	{"change owner id only", 703, 701},
	{"change group id only", -1, 704},
	{"change group id only", 703, 705},
	{"no change", -1, -1}
};

void run(unsigned int i)
{
	struct stat stat_buf;
	uid_t expect_uid = tc[i].uid == (uid_t)-1 ? tc[i - 1].uid : tc[i].uid;
	gid_t expect_gid = tc[i].gid == (uid_t)-1 ? tc[i - 1].gid : tc[i].gid;
	TST_EXP_PASS(FCHOWN(fd, tc[i].uid, tc[i].gid), "fchown(%i, %i, %i), %s",
		     fd, tc[i].uid, tc[i].gid, tc[i].desc);
	fstat(fd, &stat_buf);
	if (stat_buf.st_uid != expect_uid || stat_buf.st_gid != expect_gid) {
		tst_res(TFAIL, "%s: incorrect ownership set, expected %d %d",
			TESTFILE, expect_uid, expect_gid);
	}
}

void setup(void)
{
	fd = open(TESTFILE, O_RDWR | O_CREAT, FILE_MODE);
}

void cleanup(void)
{
	if (fd > 0)
		close(fd);
}

void main(){
	setup();
	cleanup();
}
