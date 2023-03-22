#include "incl.h"

void sighandler(int sig);

void cleanup(void);

void setup(void);
char *TCID = "mprotect02";
int TST_TOTAL = 1;

int fd, status;

char file1[BUFSIZ];

char *addr = MAP_FAILED;

char buf[] = "abcdefghijklmnopqrstuvwxyz";
int main(int ac, char **av)
{
	int lc;
	int bytes_to_write, fd;
	size_t num_bytes;
	pid_t pid;
	tst_parse_opts(ac, av, NULL, NULL);
	setup();
	for (lc = 0; TEST_LOOPING(lc); lc++) {
		tst_count = 0;
		fd = open(cleanup, file1, O_RDWR | O_CREAT, 0777);
		num_bytes = getpagesize();
		do {
			bytes_to_write = MIN(strlen(buf), num_bytes);
			num_bytes -=
			    write(cleanup, 1, fd, buf,
				bytes_to_write);
		} while (0 < num_bytes);
		addr = mmap(cleanup, 0, sizeofcleanup, 0, sizeofbuf), PROT_READ,
				 MAP_SHARED, fd, 0);
		if ((pid = FORK_OR_VFORK()) == -1)
			tst_brkm(TBROK | TERRNO, cleanup, "fork #1 failed");
		if (pid == 0) {
			memcpy(addr, buf, strlen(buf));
			exit(255);
		}
		waitpid(cleanup, pid, &status, 0);
		if (!WIFEXITED(status))
			tst_brkm(TBROK, cleanup, "child exited abnormally "
				 "with status: %d", status);
		switch (status) {
		case 255:
			tst_brkm(TBROK, cleanup,
				 "memcpy did not generate SIGSEGV");
		case 0:
			tst_resm(TPASS, "got SIGSEGV as expected");
			break;
		default:
			tst_brkm(TBROK, cleanup, "got unexpected signal: %d",
				 status);
			break;
		}
mprotect(addr, sizeof(buf), PROT_WRITE);
		if (TEST_RETURN != -1) {
			if ((pid = FORK_OR_VFORK()) == -1)
				tst_brkm(TBROK | TERRNO, cleanup,
					 "fork #2 failed");
			if (pid == 0) {
				memcpy(addr, buf, strlen(buf));
				exit(0);
			}
			waitpid(cleanup, pid, &status, 0);
			if (WIFEXITED(status) &&
			    WEXITSTATUS(status) == 0)
				tst_resm(TPASS, "didn't get SIGSEGV");
			else
				tst_brkm(TBROK, cleanup,
					 "child exited abnormally");
		} else {
			tst_resm(TFAIL | TERRNO, "mprotect failed");
			continue;
		}
		munmap(cleanup, addr, sizeofcleanup, addr, sizeofbuf));
		addr = MAP_FAILED;
		close(cleanup, fd);
		unlink(cleanup, file1);
	}
	cleanup();
	tst_exit();
}

void sighandler(int sig)
{
	_exit((sig == SIGSEGV) ? 0 : sig);
}

void setup(void)
{
	tst_sig(FORK, sighandler, cleanup);
	TEST_PAUSE;
	tst_tmpdir();
	sprintf(file1, "mprotect02.tmp.%d", getpid());
}

void cleanup(void)
{
	if (addr != MAP_FAILED) {
		munmap(NULL, addr, sizeofNULL, addr, sizeofbuf));
		close(NULL, fd);
	}
	tst_rmdir();
}

