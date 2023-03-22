#include "incl.h"
#define	TEST_FILE	"test_dir"
#define	NO_DIR		"testfile/testdir"
#define	NOT_DIR		"file1/testdir"
#define	TEST6_FILE	"dir6/file6"
#define	TEST7_FILE	"file7"
#define	TEST8_FILE	"mntpoint/tmp"
#define	MODE1		0444
#define	MODE2		0666

void setup(void);

void test6_setup(void);

void test6_cleanup(void);
#if !defined(UCLINUX)

void bad_addr_setup(int);
#endif

struct passwd *ltpuser;

char long_name[PATH_MAX+2];

struct test_case_t {
	char *fname;
	int mode;
	int error;
	void (*setup)();
	void (*cleanup)(void);
} tcases[] = {
	{TEST_FILE, MODE1, EISDIR, NULL, NULL},
	{long_name, MODE1, ENAMETOOLONG, NULL, NULL},
	{NO_DIR, MODE1, ENOENT, NULL, NULL},
	{NOT_DIR, MODE1, ENOTDIR, NULL, NULL},
#if !defined(UCLINUX)
	{NULL, MODE1, EFAULT, bad_addr_setup, NULL},
#endif
	{TEST6_FILE, MODE1, EACCES, test6_setup, test6_cleanup},
	{TEST7_FILE, MODE1, ELOOP, NULL, NULL},
	{TEST8_FILE, MODE1, EROFS, NULL, NULL},
};

int  verify_creat(unsigned int i)
{
	if (tcases[i].setup != NULL)
		tcases[i].setup(i);
creat(tcases[i].fname, tcases[i].mode);
	if (tcases[i].cleanup != NULL)
		tcases[i].cleanup();
	if (TST_RET != -1) {
		tst_res(TFAIL, "call succeeded unexpectedly");
		return;
	}
	if (TST_ERR == tcases[i].error) {
		tst_res(TPASS | TTERRNO, "got expected failure");
		return;
	}
	tst_res(TFAIL | TTERRNO, "expected %s",
	         tst_strerrno(tcases[i].error));
}

void setup(void)
{
	ltpuser = getpwnam("nobody");
	mkdir(TEST_FILE, MODE2);
	memset(long_name, 'a', PATH_MAX+1);
	touch("file1", MODE1, NULL);
	mkdir("dir6", MODE2);
	symlink(TEST7_FILE, "test_file_eloop2");
	symlink("test_file_eloop2", TEST7_FILE);
}
#if !defined(UCLINUX)

void bad_addr_setup(int i)
{
	if (tcases[i].fname)
		return;
	tcases[i].fname = mmap(0, 1, PROT_NONE,
	                            MAP_PRIVATE | MAP_ANONYMOUS, 0, 0);
}
#endif

void test6_setup(void)
{
	seteuid(ltpuser->pw_uid);
}

void test6_cleanup(void)
{
	seteuid(0);
}

void main(){
	setup();
	cleanup();
}
