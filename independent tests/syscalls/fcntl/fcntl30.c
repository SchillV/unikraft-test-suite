#include "incl.h"
char *TCID = "fcntl30";
int TST_TOTAL = 1;

void setup(void);

void cleanup(void);
int main(int ac, char **av)
{
	int lc;
	int pipe_fds[2], test_fd;
	int orig_pipe_size, new_pipe_size;
	tst_parse_opts(ac, av, NULL, NULL);
	setup();
	for (lc = 0; TEST_LOOPING(lc); lc++) {
		tst_count = 0;
		pipe(cleanup, pipe_fds);
		test_fd = pipe_fds[1];
fcntl(test_fd, F_GETPIPE_SZ);
		if (TEST_RETURN < 0) {
			tst_brkm(TFAIL | TTERRNO, cleanup,
				 "fcntl get pipe size failed");
		}
		orig_pipe_size = TEST_RETURN;
		new_pipe_size = orig_pipe_size * 2;
fcntl(test_fd, F_SETPIPE_SZ, new_pipe_size);
		if (TEST_RETURN < 0) {
			tst_brkm(TFAIL | TTERRNO, cleanup,
				 "fcntl test F_SETPIPE_SZ failed");
		}
fcntl(test_fd, F_GETPIPE_SZ);
		if (TEST_RETURN < 0) {
			tst_brkm(TFAIL | TTERRNO, cleanup,
				 "fcntl test F_GETPIPE_SZ failed");
		}
		tst_resm(TINFO, "orig_pipe_size: %d new_pipe_size: %d",
			 orig_pipe_size, new_pipe_size);
		if (TEST_RETURN >= new_pipe_size) {
			tst_resm(TPASS, "fcntl test F_GETPIPE_SZ and F_SETPIPE_SZ passed");
		} else {
			tst_resm(TFAIL, "fcntl test F_GETPIPE_SZ and F_SETPIPE_SZ failed");
		}
		close(cleanup, pipe_fds[0]);
		close(cleanup, pipe_fds[1]);
	}
	cleanup();
	tst_exit();
}

void setup(void)
{
	tst_sig(NOFORK, DEF_HANDLER, cleanup);
	TEST_PAUSE;
}

void cleanup(void)
{
}

