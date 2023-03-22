#include "incl.h"

void setup(void);

void cleanup(void);
char *TCID = "fcntl08";
int TST_TOTAL = 1;

int fd;
int main(int ac, char **av)
{
	int lc;
	tst_parse_opts(ac, av, NULL, NULL);
	setup();
	for (lc = 0; TEST_LOOPING(lc); lc++) {
		tst_count = 0;
fcntl(fd, F_SETFL, O_NDELAY | O_APPEND | O_NONBLOCK);
		if (TEST_RETURN == -1) {
			tst_resm(TFAIL | TTERRNO, "fcntl failed");
		} else {
			tst_resm(TPASS, "fcntl returned %ld",
				 TEST_RETURN);
		}
	}
	cleanup();
	tst_exit();
}
void setup(void)
{
	tst_sig(NOFORK, DEF_HANDLER, cleanup);
	TEST_PAUSE;
	tst_tmpdir();
	fd = open(cleanup, "test_file", O_RDWR | O_CREAT, 0700);
}
void cleanup(void)
{
	if (close(fd) == -1)
		tst_resm(TWARN | TERRNO, "close failed");
	tst_rmdir();
}

