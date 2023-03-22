#include "incl.h"
void cleanup(void);
void setup(void);
void sighandler(int sig);
void do_child(void);
char *TCID = "kill07";
int TST_TOTAL = 1;
int shmid1;
extern key_t semkey;
int *flag;
extern int getipckey();
extern void rm_shm(int);
#define TEST_SIG SIGKILL
int main(int ac, char **av)
{
	int lc;
	pid_t pid;
	int exno, status, nsig, asig, ret;
	struct sigaction my_act, old_act;
	tst_parse_opts(ac, av, NULL, NULL);
#ifdef UCLINUX
	maybe_run_child(&do_child, "");
#endif
	for (lc = 0; TEST_LOOPING(lc); lc++) {
		tst_count = 0;
		status = 1;
		exno = 1;
		my_act.sa_handler = sighandler;
		my_act.sa_flags = SA_RESTART;
		sigemptyset(&my_act.sa_mask);
		if ((shmid1 = shmget(semkey, (int)getpagesize(),
				     0666 | IPC_CREAT)) == -1) {
			tst_brkm(TBROK, cleanup,
				 "Failed to setup shared memory");
		}
		if (*(flag = shmat(shmid1, 0, 0)) == -1) {
			tst_brkm(TBROK, cleanup,
				 "Failed to attatch shared memory:%d", *flag);
		}
		*flag = 0;
		ret = sigaction(TEST_SIG, &my_act, &old_act);
		pid = FORK_OR_VFORK();
		if (pid < 0) {
			tst_brkm(TBROK, cleanup, "Fork of child failed");
		} else if (pid == 0) {
#ifdef UCLINUX
			if (self_exec(av[0], "") < 0) {
				tst_brkm(TBROK, cleanup,
					 "self_exec of child failed");
			}
#else
			do_child();
#endif
		} else {
			sleep(1);
kill(pid, TEST_SIG);
			waitpid(pid, &status, 0);
		}
		if (TEST_RETURN == -1) {
			tst_brkm(TFAIL, cleanup, "%s failed - errno = %d : %s",
				 TCID, TEST_ERRNO, strerror(TEST_ERRNO));
		}
		 * Check to see if the process was terminated with the
		 * expected signal.
		 */
		nsig = WTERMSIG(status);
		asig = WIFSIGNALED(status);
		if ((asig == 0) & (*flag == 1)) {
			tst_resm(TFAIL, "SIGKILL was unexpectedly"
				 " caught");
		} else if ((asig == 1) & (nsig == TEST_SIG)) {
			tst_resm(TINFO, "received expected signal %d",
				 nsig);
			tst_resm(TPASS,
				 "Did not catch signal as expected");
		} else if (nsig) {
			tst_resm(TFAIL,
				 "expected signal %d received %d",
				 TEST_SIG, nsig);
		} else {
			tst_resm(TFAIL, "No signals received");
		}
		if (shmdt(flag)) {
			tst_brkm(TBROK, cleanup, "shmdt failed ");
		}
	}
	cleanup();
	tst_exit();
}
void sighandler(int sig)
{
	return;
}
void do_child(void)
{
	int exno = 1;
	sleep(300);
	tst_resm(TINFO, "Child never received a signal");
	exit(exno);
}
void setup(void)
{
	TEST_PAUSE;
	 * Create a temporary directory and cd into it.
	 * This helps to ensure that a unique msgkey is created.
	 * See libs/libltpipc/libipc.c for more information.
	 */
	tst_tmpdir();
	semkey = getipckey();
}
void cleanup(void)
{
	 * remove the shared memory
	 */
	rm_shm(shmid1);
	tst_rmdir();
}

