#include "incl.h"
#define TESTFILE	"test_file"

int fd_ok;

int fd_ebadf = -1;

struct stat stat_buf;

struct tcase {
	int *fd;
	struct stat *stat_buf;
	int exp_err;
} tcases[] = {
	{&fd_ebadf, &stat_buf, EBADF},
	{&fd_ok, NULL, EFAULT},
};

void check_fstat(unsigned int tc_num)
{
	struct tcase *tc = &tcases[tc_num];
fstat(*tc->fd, tc->stat_buf);
	if (TST_RET == -1) {
		if (tc->exp_err == TST_ERR) {
			tst_res(TPASS,
				 "fstat() fails with expected error %s",
				 tst_strerrno(tc->exp_err));
		} else {
			tst_res(TFAIL | TTERRNO,
				 "fstat() did not fail with %s, but with",
				 tst_strerrno(tc->exp_err));
		}
	} else {
		tst_res(TFAIL, "fstat() returned %ld, expected -1",
			 TST_RET);
	}
}

void run(unsigned int tc_num)
{
	pid_t pid;
	int status;
	pid = fork();
	if (pid == 0) {
		check_fstat(tc_num);
		return;
	}
	waitpid(pid, &status, 0);
	if (tcases[tc_num].exp_err == EFAULT && WTERMSIG(status) == SIGSEGV) {
		tst_res(TPASS, "fstat() failed as expected with SIGSEGV");
		return;
	}
	if (WIFEXITED(status) && WEXITSTATUS(status) == 0)
		return;
	tst_res(TFAIL, "child %s", tst_strstatus(status));
}

void setup(void)
{
	fd_ok = open(TESTFILE, O_RDWR | O_CREAT, 0644);
}

void cleanup(void)
{
	if (fd_ok > 0)
		close(fd_ok);
}

void main(){
	setup();
	cleanup();
}
