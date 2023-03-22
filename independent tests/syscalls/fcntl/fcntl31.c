#include "incl.h"

void setup(void);

void cleanup(void);

void setown_pid_test(void);

void setown_pgrp_test(void);
#if defined(HAVE_STRUCT_F_OWNER_EX)

void setownex_tid_test(void);

void setownex_pid_test(void);

void setownex_pgrp_test(void);

struct f_owner_ex orig_own_ex;
#endif

void signal_parent(void);

void check_io_signal(char *des);

void test_set_and_get_sig(int sig, char *des);

pid_t pid;

pid_t orig_pid;

pid_t pgrp_pid;

struct timespec timeout;

sigset_t newset, oldset;

int test_fd;

int pipe_fds[2];

void (*testfunc[])(void) = {
	setown_pid_test, setown_pgrp_test,
#if defined(HAVE_STRUCT_F_OWNER_EX)
	setownex_tid_test, setownex_pid_test, setownex_pgrp_test
#endif
};
char *TCID = "fcntl31";
int TST_TOTAL = ARRAY_SIZE(testfunc);
int main(int ac, char **av)
{
	int lc, i;
	tst_parse_opts(ac, av, NULL, NULL);
	setup();
	for (lc = 0; TEST_LOOPING(lc); lc++) {
		tst_count = 0;
		for (i = 0; i < TST_TOTAL; i++)
			(*testfunc[i])();
	}
	cleanup();
	tst_exit();
}

void setup(void)
{
	int ret;
	tst_sig(FORK, DEF_HANDLER, cleanup);
	TEST_PAUSE;
	pipe(cleanup, pipe_fds);
	test_fd = pipe_fds[0];
	if (fcntl(test_fd, F_SETFL, O_ASYNC) < 0)
		tst_brkm(TBROK | TERRNO, cleanup, "fcntl set O_ASYNC failed");
	pid = getpid();
	if (pid != getsid(0)) {
		ret = setpgrp();
		if (ret < 0)
			tst_brkm(TBROK | TERRNO, cleanup, "setpgrp() failed");
	}
	pgrp_pid = getpgid(0);
	if (pgrp_pid < 0)
		tst_brkm(TBROK | TERRNO, cleanup, "getpgid() failed");
#if defined(HAVE_STRUCT_F_OWNER_EX)
fcntl(test_fd, F_GETOWN_EX, &orig_own_ex);
	if (TEST_RETURN < 0) {
		tst_brkm(TFAIL | TTERRNO, cleanup,
			 "fcntl get original f_owner_ex info failed");
	}
#endif
fcntl(test_fd, F_GETOWN);
	if (TEST_RETURN < 0) {
		tst_brkm(TFAIL | TTERRNO, cleanup,
			 "fcntl get original pid info failed");
	}
	orig_pid = TEST_RETURN;
	sigemptyset(&newset);
	sigaddset(&newset, SIGUSR1);
	sigaddset(&newset, SIGIO);
	if (sigprocmask(SIG_SETMASK, &newset, &oldset) < 0)
		tst_brkm(TBROK | TERRNO, cleanup, "sigprocmask failed");
	timeout.tv_sec = 5;
	timeout.tv_nsec = 0;
}

void setown_pid_test(void)
{
fcntl(test_fd, F_SETOWN, pid);
	if (TEST_RETURN < 0) {
		tst_brkm(TFAIL | TTERRNO, cleanup,
			 "fcntl(F_SETOWN) set process id failed");
	}
	test_set_and_get_sig(SIGUSR1, "F_GETOWN, F_SETOWN for process ID");
fcntl(test_fd, F_SETOWN, orig_pid);
	if (TEST_RETURN < 0) {
		tst_brkm(TFAIL | TTERRNO, cleanup,
			 "fcntl(F_SETOWN) restore orig_pid failed");
	}
}

void setown_pgrp_test(void)
{
fcntl(test_fd, F_SETOWN, -pgrp_pid);
	if (TEST_RETURN < 0) {
		tst_brkm(TFAIL | TTERRNO, cleanup,
			 "fcntl(F_SETOWN) set process group id failed");
	}
	test_set_and_get_sig(SIGUSR1,
			     "F_GETOWN, F_SETOWN for process group ID");
fcntl(test_fd, F_SETOWN, orig_pid);
	if (TEST_RETURN < 0) {
		tst_brkm(TFAIL | TTERRNO, cleanup,
			 "fcntl(F_SETOWN) restore orig_pid failed");
	}
}
#if defined(HAVE_STRUCT_F_OWNER_EX)

void setownex_cleanup(void)
{
fcntl(test_fd, F_SETOWN_EX, &orig_own_ex);
	if (TEST_RETURN < 0) {
		tst_brkm(TFAIL | TTERRNO, cleanup,
			 "fcntl F_SETOWN_EX restore orig_own_ex failed");
	}
}

void setownex_tid_test(void)
{
	

struct f_owner_ex tst_own_ex;
	tst_own_ex.type = F_OWNER_TID;
	tst_own_ex.pid = tst_syscall(__NR_gettid);
fcntl(test_fd, F_SETOWN_EX, &tst_own_ex);
	if (TEST_RETURN < 0) {
		tst_brkm(TFAIL | TTERRNO, cleanup,
			 "fcntl F_SETOWN_EX failed");
	}
	test_set_and_get_sig(SIGUSR1, "F_GETOWN_EX, F_SETOWN_EX for thread ID");
	setownex_cleanup();
}

void setownex_pid_test(void)
{
	

struct f_owner_ex tst_own_ex;
	tst_own_ex.type = F_OWNER_PID;
	tst_own_ex.pid = pid;
fcntl(test_fd, F_SETOWN_EX, &tst_own_ex);
	if (TEST_RETURN < 0) {
		tst_brkm(TFAIL | TTERRNO, cleanup,
			 "fcntl F_SETOWN_EX failed");
	}
	test_set_and_get_sig(SIGUSR1,
			     "F_GETOWN_EX, F_SETOWN_EX for process ID");
	setownex_cleanup();
}

void setownex_pgrp_test(void)
{
	

struct f_owner_ex tst_own_ex;
	tst_own_ex.type = F_OWNER_PGRP;
	tst_own_ex.pid = pgrp_pid;
fcntl(test_fd, F_SETOWN_EX, &tst_own_ex);
	if (TEST_RETURN < 0) {
		tst_brkm(TFAIL | TTERRNO, cleanup,
			 "fcntl F_SETOWN_EX failed");
	}
	test_set_and_get_sig(SIGUSR1,
			     "F_GETOWN_EX, F_SETOWN_EX for process group ID");
	setownex_cleanup();
}
#endif

void test_set_and_get_sig(int sig, char *des)
{
	int orig_sig;
fcntl(test_fd, F_GETSIG);
	if (TEST_RETURN < 0) {
		tst_brkm(TFAIL | TTERRNO, cleanup,
			 "fcntl(fd, F_GETSIG) get orig_sig failed");
	}
	orig_sig = TEST_RETURN;
	if (orig_sig == 0 || orig_sig == SIGIO)
		tst_resm(TINFO, "default io events signal is SIGIO");
fcntl(test_fd, F_SETSIG, sig);
	if (TEST_RETURN < 0) {
		tst_brkm(TFAIL | TTERRNO, cleanup,
			 "fcntl(fd, F_SETSIG, SIG: %d) failed", sig);
	}
fcntl(test_fd, F_GETSIG);
	if (TEST_RETURN < 0) {
		tst_brkm(TFAIL | TTERRNO, cleanup,
			 "fcntl(fd, F_GETSIG) get the set signal failed");
	}
	if (TEST_RETURN != sig) {
		tst_brkm(TFAIL | TTERRNO, cleanup,
			 "fcntl F_SETSIG set SIG: %d failed", sig);
	}
	check_io_signal(des);
fcntl(test_fd, F_SETSIG, orig_sig);
	if (TEST_RETURN < 0) {
		tst_brkm(TFAIL | TTERRNO, cleanup,
			 "fcntl restore default signal failed");
	}
}

void signal_parent(void)
{
	int ret, fd;
	fd = pipe_fds[1];
	close(pipe_fds[0]);
	ret = setpgrp();
	if (ret < 0) {
		fprintf(stderr, "child process(%d) setpgrp() failed: %s \n",
			getpid(), strerror(errno));
	}
	tst_process_state_wait2(getppid(), 'S');
	ret = write(fd, "c", 1);
	switch (ret) {
	case 0:
		fprintf(stderr, "No data written, something is wrong\n");
	break;
	case -1:
		fprintf(stderr, "Failed to write to pipe: %s\n",
			strerror(errno));
	break;
	}
	close(fd);
	return;
}

void check_io_signal(char *des)
{
	int ret;
	char c;
	pid_t child;
	child = tst_fork();
	if (child < 0)
		tst_brkm(TBROK | TERRNO, cleanup, "fork failed");
	if (child == 0) {
		signal_parent();
		exit(0);
	} else {
		ret = sigtimedwait(&newset, NULL, &timeout);
		if (ret == -1) {
			tst_brkm(TBROK | TERRNO, NULL,
				 "sigtimedwait() failed.");
		}
		switch (ret) {
		case SIGUSR1:
			tst_resm(TPASS, "fcntl test %s success", des);
		break;
		case SIGIO:
			tst_resm(TFAIL, "received default SIGIO, fcntl test "
				 "%s failed", des);
		break;
		default:
			tst_brkm(TBROK, cleanup, "fcntl io events "
				 "signal mechanism work abnormally");
		}
		read(cleanup, 1, test_fd, &c, 1);
		wait(NULL);
	}
}

void cleanup(void)
{
	if (sigprocmask(SIG_SETMASK, &oldset, NULL) < 0)
		tst_resm(TWARN | TERRNO, "sigprocmask restore oldset failed");
	if (pipe_fds[0] > 0 && close(pipe_fds[0]) == -1)
		tst_resm(TWARN | TERRNO, "close(%d) failed", pipe_fds[0]);
	if (pipe_fds[1] > 0 && close(pipe_fds[1]) == -1)
		tst_resm(TWARN | TERRNO, "close(%d) failed", pipe_fds[1]);
}

