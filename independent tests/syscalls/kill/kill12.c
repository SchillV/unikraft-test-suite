#include "incl.h"
>KEYS:  < kill(), wait(), signal()
>WHAT:  < Check that when a child is killed by its parent, it returns the
	< correct values to the waiting parent--the child sets signal to
	< ignore the kill
>HOW:   < For each signal: Send that signal to a child that has elected
	< to catch the signal, check that the correct status was returned
	< to the waiting parent.
	< NOTE: Signal 9 (kill) is not catchable, and must be dealt with
	< separately.
>BUGS:  < None known
======================================================================*/
#ifndef _GNU_SOURCE
#define _GNU_SOURCE 1
#endif
#define ITER    3
#define FAILED 0
#define PASSED 1
char *TCID = "kill12";
int local_flag = PASSED;
int block_number;
FILE *temp;
int TST_TOTAL = 1;

int sig;
int anyfail();
int blenter();
int instress();
void setup();
void terror();
void fail_exit();
void ok_exit();
int forkfail();
void do_child();
int chflag;
int main(int argc, char **argv)
{
	int pid, npid;
	int nsig, exno, nexno, status;
	int ret_val = 0;
	int core;
	void chsig();
	tst_parse_opts(argc, argv, NULL, NULL);
	setup();
	blenter();
	exno = 1;
	if (sigset(SIGCHLD, chsig) == SIG_ERR) {
		fprintf(temp, "\tsigset failed, errno = %d\n", errno);
		fail_exit();
	}
	for (sig = 1; sig < 14; sig++) {
		fflush(temp);
		chflag = 0;
		pid = FORK_OR_VFORK();
		if (pid < 0) {
			forkfail();
		}
		if (pid == 0) {
			do_child();
		} else {
				sleep(1);
#ifdef BCS
			while ((npid = wait(&status)) != pid
			       || (npid == -1 && errno == EINTR)) ;
			if (npid != pid) {
				fprintf(temp,
					"wait error: wait returned wrong pid\n");
				ret_val = 1;
			}
#else
			while ((npid = waitpid(pid, &status, 0)) != -1
			       || errno == EINTR) ;
#endif
			   nsig = status & 0177;
			   core = status & 0200;
			   nexno = (status & 0xff00) >> 8;
			 */
			nsig = WTERMSIG(status);
#ifdef WCOREDUMP
			core = WCOREDUMP(status);
#endif
			nexno = WIFEXITED(status);
			   it should be 0, except when sig = 9          */
			if ((sig == 9) && (nsig != sig)) {
				fprintf(temp, "wait error: unexpected signal"
					" returned when the signal sent was 9"
					" The status of the process is %d \n",
					status);
				ret_val = 1;
			}
			if ((sig != 9) && (nsig != 0)) {
				fprintf(temp, "wait error: unexpected signal "
					"returned, the status of the process is "
					"%d  \n", status);
				ret_val = 1;
			}
			   it should be 1, except when sig = 9          */
			if (sig == 9)
				if (nexno != 0) {
					fprintf(temp, "signal error: unexpected"
						" exit number returned when"
						" signal sent was 9, the status"
						" of the process is %d \n",
						status);
					ret_val = 1;
				} else;
			else if (nexno != 1) {
				fprintf(temp, "signal error: unexpected exit "
					"number returned,the status of the"
					" process is %d\n", status);
				ret_val = 1;
			}
		}
	}
	if (ret_val)
		local_flag = FAILED;
	anyfail();
	tst_exit();
}
void chsig(void)
{
	chflag++;
}
int anyfail(void)
{
	(local_flag == FAILED) ? tst_resm(TFAIL,
					  "Test failed") : tst_resm(TPASS,
								    "Test passed");
	tst_exit();
}
void do_child(void)
{
	int exno = 1;
	while (!chflag)
	exit(exno);
}
void setup(void)
{
	temp = stderr;
}
int blenter(void)
{
	local_flag = PASSED;
	return 0;
}
void terror(char *message)
{
	tst_resm(TBROK, "Reason: %s:%s", message, strerror(errno));
}
void fail_exit(void)
{
	local_flag = FAILED;
	anyfail();
}
int forkfail(void)
{
	tst_brkm(TBROK, NULL, "FORK FAILED - terminating test.");
}

