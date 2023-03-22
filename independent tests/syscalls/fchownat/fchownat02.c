#include "incl.h"
#define _GNU_SOURCE
#define TESTFILE	"testfile"
#define TESTFILE_LINK	"testfile_link"
char *TCID = "fchownat02";
int TST_TOTAL = 1;

int dir_fd;

uid_t set_uid = 1000;

gid_t set_gid = 1000;

void setup(void);

void cleanup(void);

int  test_verify(

int  fchownat_verify(
int main(int ac, char **av)
{
	int lc;
	int i;
	tst_parse_opts(ac, av, NULL, NULL);
	setup();
	for (lc = 0; TEST_LOOPING(lc); lc++) {
		tst_count = 0;
		for (i = 0; i < TST_TOTAL; i++)
int 			fchownat_verify();
	}
	cleanup();
	tst_exit();
}

void setup(void)
{
	struct stat c_buf, l_buf;
	tst_require_root();
	tst_sig(NOFORK, DEF_HANDLER, cleanup);
	TEST_PAUSE;
	tst_tmpdir();
	dir_fd = open(cleanup, "./", O_DIRECTORY);
	touch(cleanup, TESTFILE, 0600, NULL);
	symlink(cleanup, TESTFILE, TESTFILE_LINK);
	stat(cleanup, TESTFILE_LINK, &c_buf);
	lstat(cleanup, TESTFILE_LINK, &l_buf);
	if (l_buf.st_uid == set_uid || l_buf.st_gid == set_gid) {
		tst_brkm(TBROK | TERRNO, cleanup,
			 "link_uid(%d) == set_uid(%d) or link_gid(%d) == "
			 "set_gid(%d)", l_buf.st_uid, set_uid, l_buf.st_gid,
			 set_gid);
	}
}

int  fchownat_verify(
{
fchownat(dir_fd, TESTFILE_LINK, set_uid, set_gi;
		      AT_SYMLINK_NOFOLLOW));
	if (TEST_RETURN != 0) {
		tst_resm(TFAIL | TTERRNO, "fchownat() failed, errno=%d : %s",
			 TEST_ERRNO, strerror(TEST_ERRNO));
	} else {
int 		test_verify();
	}
}

int  test_verify(
{
	struct stat c_buf, l_buf;
	stat(cleanup, TESTFILE_LINK, &c_buf);
	lstat(cleanup, TESTFILE_LINK, &l_buf);
	if (c_buf.st_uid != set_uid && l_buf.st_uid == set_uid &&
	    c_buf.st_gid != set_gid && l_buf.st_gid == set_gid) {
		tst_resm(TPASS, "fchownat() test AT_SYMLINK_NOFOLLOW success");
	} else {
		tst_resm(TFAIL,
			 "fchownat() test AT_SYMLINK_NOFOLLOW fail with uid=%d "
			 "link_uid=%d set_uid=%d | gid=%d link_gid=%d "
			 "set_gid=%d", c_buf.st_uid, l_buf.st_uid, set_uid,
			 c_buf.st_gid, l_buf.st_gid, set_gid);
	}
}

void cleanup(void)
{
	tst_rmdir();
}

