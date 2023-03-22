#include "incl.h"
#ifdef HAVE_SYS_XATTR_H
# include <sys/xattr.h>
#endif
char *TCID = "getxattr01";
#ifdef HAVE_SYS_XATTR_H
#define XATTR_TEST_KEY "user.testkey"
#define XATTR_TEST_VALUE "this is a test value"
#define XATTR_TEST_VALUE_SIZE 20
#define BUFFSIZE 64

void setup(void);

void cleanup(void);
char filename[BUFSIZ];
struct test_case {
	char *fname;
	char *key;
	char *value;
	size_t size;
	int exp_err;
};
struct test_case tc[] = {
	 .fname = filename,
	 .key = "user.nosuchkey",
	 .value = NULL,
	 .size = BUFFSIZE - 1,
	 .exp_err = ENODATA,
	 },
	 .fname = filename,
	 .key = XATTR_TEST_KEY,
	 .value = NULL,
	 .size = 1,
	 .exp_err = ERANGE,
	 },
	 .fname = filename,
	 .key = XATTR_TEST_KEY,
	 .value = NULL,
	 .size = BUFFSIZE - 1,
	 .exp_err = 0,
	 },
};
int TST_TOTAL = sizeof(tc) / sizeof(tc[0]) + 1;
int main(int argc, char *argv[])
{
	int lc;
	int i;
	tst_parse_opts(argc, argv, NULL, NULL);
	setup();
	for (lc = 0; TEST_LOOPING(lc); lc++) {
		tst_count = 0;
		for (i = 0; i < (int)ARRAY_SIZE(tc); i++) {
getxattr(tc[i].fname, tc[i].key, tc[i].valu;
				      tc[i].size));
			if (TEST_ERRNO == tc[i].exp_err) {
				tst_resm(TPASS | TTERRNO, "expected behavior");
			} else {
				tst_resm(TFAIL | TTERRNO, "unexpected behavior"
					 "- expected errno %d - Got",
					 tc[i].exp_err);
			}
		}
		if (TEST_RETURN != XATTR_TEST_VALUE_SIZE) {
			tst_resm(TFAIL,
				 "getxattr() returned wrong size %ld expected %d",
				 TEST_RETURN, XATTR_TEST_VALUE_SIZE);
			continue;
		}
		if (memcmp(tc[i - 1].value, XATTR_TEST_VALUE, XATTR_TEST_VALUE_SIZE))
			tst_resm(TFAIL, "Wrong value, expect \"%s\" got \"%s\"",
				 XATTR_TEST_VALUE, tc[i - 1].value);
		else
			tst_resm(TPASS, "Got the right value");
	}
	cleanup();
	tst_exit();
}

void setup(void)
{
	int fd;
	unsigned int i;
	tst_require_root();
	tst_tmpdir();
	snprintf(filename, BUFSIZ, "getxattr01testfile");
	fd = creat(cleanup, filename, 0644);
	close(fd);
	if (setxattr(filename, XATTR_TEST_KEY, XATTR_TEST_VALUE,
		     strlen(XATTR_TEST_VALUE), XATTR_CREATE) == -1) {
		if (errno == ENOTSUP) {
			tst_brkm(TCONF, cleanup, "No xattr support in fs or "
				 "mount without user_xattr option");
		}
	}
	for (i = 0; i < ARRAY_SIZE(tc); i++) {
		tc[i].value = malloc(BUFFSIZE);
		if (tc[i].value == NULL) {
			tst_brkm(TBROK | TERRNO, cleanup,
				 "Cannot allocate memory");
		}
	}
	TEST_PAUSE;
}

void cleanup(void)
{
	tst_rmdir();
}
int main(int argc, char *argv[])
{
	tst_brkm(TCONF, NULL, "<sys/xattr.h> does not exist.");
}
#endif

