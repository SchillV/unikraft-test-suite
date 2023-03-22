#include "incl.h"

void setup(void);

void cleanup(void);

struct pathconf_args {
	char *name;
	int value;
} test_cases[] = {
	{"_PC_MAX_CANON", _PC_MAX_CANON},
	{"_PC_MAX_INPUT", _PC_MAX_INPUT},
	{"_PC_VDISABLE", _PC_VDISABLE},
	{"_PC_LINK_MAX", _PC_LINK_MAX},
	{"_PC_NAME_MAX", _PC_NAME_MAX},
	{"_PC_PATH_MAX", _PC_PATH_MAX},
	{"_PC_PIPE_BUF", _PC_PIPE_BUF},
	{"_PC_CHOWN_RESTRICTED", _PC_CHOWN_RESTRICTED},
	{"_PC_NO_TRUNC", _PC_NO_TRUNC},
};
char *TCID = "fpathconf01";
int TST_TOTAL = ARRAY_SIZE(test_cases);

int fd;
int main(int ac, char **av)
{
	int lc;
	int i;
	tst_parse_opts(ac, av, NULL, NULL);
	setup();
	for (lc = 0; TEST_LOOPING(lc); lc++) {
		tst_count = 0;
		for (i = 0; i < TST_TOTAL; i++) {
			errno = 0;
fpathconf(fd, test_cases[i].value);
			if (TEST_RETURN == -1) {
				if (TEST_ERRNO == 0) {
					tst_resm(TINFO,
						 "fpathconf has NO limit for "
						 "%s", test_cases[i].name);
				} else {
					tst_resm(TFAIL | TTERRNO,
						 "fpathconf(fd, %s) failed",
						 test_cases[i].name);
				}
			} else {
				tst_resm(TPASS,
					 "fpathconf(fd, %s) returned %ld",
					 test_cases[i].name, TEST_RETURN);
			}
		}
	}
	cleanup();
	tst_exit();
}

void setup(void)
{
	tst_sig(FORK, DEF_HANDLER, cleanup);
	TEST_PAUSE;
	tst_tmpdir();
	fd = open(cleanup, "fpafile01", O_RDWR | O_CREAT, 0700);
}

void cleanup(void)
{
	if (fd > 0)
		close(NULL, fd);
	tst_rmdir();
}

