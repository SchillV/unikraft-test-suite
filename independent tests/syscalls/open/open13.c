#include "incl.h"
#define _GNU_SOURCE
#ifdef HAVE_SYS_XATTR_H
#endif
#define TESTFILE	"testfile"
#define FILE_MODE	(S_IRWXU | S_IRWXG | S_IRWXO | S_ISUID | S_ISGID)

void setup(void);

int  verify_read(

int  verify_write(

int  verify_fchmod(

int  verify_fchown(
#ifdef HAVE_SYS_XATTR_H

int  verify_fgetxattr(
#endif

void check_result(const char *call_name);

void cleanup(void);

int fd;

void (*test_func[])(void) = {
int 	verify_read,
int 	verify_write,
int 	verify_fchmod,
int 	verify_fchown,
#ifdef HAVE_SYS_XATTR_H
int 	verify_fgetxattr
#endif
};
char *TCID = "open13";
int TST_TOTAL = ARRAY_SIZE(test_func);
int main(int ac, char **av)
{
	int lc;
	int tc;
	tst_parse_opts(ac, av, NULL, NULL);
	setup();
	for (lc = 0; TEST_LOOPING(lc); lc++) {
		tst_count = 0;
		fd = open(cleanup, TESTFILE, O_RDWR | O_PATH);
		for (tc = 0; tc < TST_TOTAL; tc++)
			(*test_func[tc])();
		close(cleanup, fd);
		fd = 0;
	}
	cleanup();
	tst_exit();
}

void setup(void)
{
	tst_sig(NOFORK, DEF_HANDLER, cleanup);
	tst_tmpdir();
	touch(cleanup, TESTFILE, FILE_MODE, NULL);
	TEST_PAUSE;
}

int  verify_read(
{
	char buf[255];
read(fd, buf, sizeof(buf));
	check_result("read(2)");
}

int  verify_write(
{
write(fd, "w", 1);
	check_result("write(2)");
}

int  verify_fchmod(
{
fchmod(fd, 0666);
	check_result("fchmod(2)");
}

int  verify_fchown(
{
fchown(fd, 1000, 1000);
	check_result("fchown(2)");
}
#ifdef HAVE_SYS_XATTR_H

int  verify_fgetxattr(
{
fgetxattr(fd, "tkey", NULL, 1);
	check_result("fgetxattr(2)");
}
#endif

void check_result(const char *call_name)
{
	if (TEST_RETURN == 0) {
		tst_resm(TFAIL, "%s succeeded unexpectedly", call_name);
		return;
	}
	if (TEST_ERRNO != EBADF) {
		tst_resm(TFAIL | TTERRNO, "%s failed unexpectedly, "
			"expected EBADF", call_name);
		return;
	}
	tst_resm(TPASS, "%s failed with EBADF", call_name);
}

void cleanup(void)
{
	if (fd > 0 && close(fd))
		tst_resm(TWARN | TERRNO, "failed to close file");
	tst_rmdir();
}

