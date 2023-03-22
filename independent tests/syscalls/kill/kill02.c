#include "incl.h"
    OS Test -  Silicon Graphics, Inc.
    TEST IDENTIFIER :  kill02  Sending a signal to processes with the same process group ID.
    PARENT DOCUMENT :  kiltds01  Kill System Call.
    AUTHOR          :  Dave Baumgartner
    CO-PILOT        :  Barrie Kletscher
    DATE STARTED    :  12/30/85
    TEST ITEMS
	1. Sending a signal to pid of zero sends to all processes whose process
	   group ID is equal to the process group ID as the sender.
	2. Sending a signal to pid of zero does not send to processes in another process group.
    OUTPUT SPECIFICATIONS
	PASS :
		kiltcs02 1 PASS The signal was sent to all processes in the process group.
		kiltcs02 2 PASS The signal was not sent to selective processes that were not in the process group.
	FAIL :
		kiltcs02 1 FAIL The signal was not sent to all processes in the process group.
		kiltcs02 2 FAIL The signal was sent to a process that was not in the process group.
	BROK :
		kiltcs02 # BROK System call XXX failed. Errno:X, Error message:XXX.
		kiltcs02 # BROK Setting to catch unexpected signal %d failed. Errno: %d, Error message %s.
		kiltcs02 # BROK Setting to ignore signal %d failed. Errno: %d, Error message %s.
	WARN :
		kiltcs02 0 WARN Unexpected signal X was caught.
    SPECIAL PROCEDURAL REQUIREMENTS
	The program must be linked with tst_res.o.
    DETAILED DESCRIPTION
	Set up unexpected signal handling.
	Set up one pipe for each process to be created with no blocking for read.
	If setup fails exit.
	Fork 2 children(1 & 2).
	Wait for set up complete messages from the 1st and 2nd child.
	Send the signal SIGUSR1 with pid equal to zero.
	Sleep a reasonable amount of time so that each child has been swapped in
	to process the signal.
	Now decide the outcome of the test items by reading from each pipe to find
	out if the child was interrupted by the signal and wrote to it.
	Remove the second child.
	Tell the first child it is time to remove it's child B because the decisions have been made.
	Exit.
	Set to catch SIGUSR1 with an int_rout1.
	Set up to handle the message from the parent to remove child B.
	Fork two children(A & B).
	Wait for set up complete messages from child A & child B.
	Send a set up complete message to the parent.
	Pause until the signal SIGUSR1 comes in from the parent.
	Pause until the parent says it is time to remove the child.
	Exit.
	Set to catch SIGUSR1 with an int_rout2.
	Set the process group to be something different than the parents.
	Send a set up complete message to the parent.
	Pause until killed by parent because this child shouldn't receive signal SIGUSR1.
	Set to catch SIGUSR1 with an int_routA.
	Send a set up complete message to the parent(First Child).
	Pause until the signal SIGUSR1 comes in from the parent.
	Exit.
	Set to catch SIGUSR1 with an int_routB.
	Set the process group to be something different than the parents(First Child's).
	Send a set up complete message to the parent.
	Pause until killed by parent because this child shouldn't receive signal SIGUSR1.
	Write to the appropriate pipe that the signal SIGUSR1 was caught.
	Remove child B.
void setup();
void cleanup();
char *TCID = "kill02";
int TST_TOTAL = 2;
#ifdef UCLINUX

char *argv0;
void childA_rout_uclinux();
void childB_rout_uclinux();
#endif
int main(int ac, char **av)
{
	int lc;
	tst_parse_opts(ac, av, NULL, NULL);
#ifdef UCLINUX
	argv0 = av[0];
	maybe_run_child(&childA_rout_uclinux, "nd", 1, &pipeA_fd[1]);
	maybe_run_child(&childB_rout_uclinux, "nd", 2, &pipeB_fd[1]);
	maybe_run_child(&child1_rout, "ndddddd", 3, &pipe1_fd[1], &pipe2_fd[1],
			&pipeA_fd[0], &pipeA_fd[1], &pipeB_fd[0], &pipeB_fd[1]);
	maybe_run_child(&child2_rout, "nd", 4, &pipe2_fd[1]);
#endif
	setup();
	for (lc = 0; TEST_LOOPING(lc); lc++) {
		tst_count = 0;
		if ((pid1 = FORK_OR_VFORK()) > 0) {
			if ((pid2 = FORK_OR_VFORK()) > 0) {
				(void)parent_rout();
			} else if (pid2 == 0) {
#ifdef UCLINUX
				if (self_exec(argv0, "nd", 4, pipe2_fd[1]) < 0) {
					if (kill(pid1, SIGKILL) == -1
					    && errno != ESRCH) {
						tst_resm(TWARN,
							 "Child process may not have been killed.");
					}
					tst_brkm(TBROK | TERRNO, cleanup,
						 "fork failed");
				}
#else
				(void)child2_rout();
#endif
			} else {
				 *  The second fork failed kill the first child.
				 */
				if (kill(pid1, SIGKILL) == -1 && errno != ESRCH) {
					tst_resm(TWARN,
						 "Child process may not have been killed.");
				}
				tst_brkm(TBROK | TERRNO, cleanup,
					 "fork failed");
			}
		} else if (pid1 == 0) {
			 *  This is child 1.
			 */
#ifdef UCLINUX
			if (self_exec
			    (argv0, "ndddddd", 3, pipe1_fd[1], pipe2_fd[1],
			     pipeA_fd[0], pipeA_fd[1], pipeB_fd[0],
			     pipeB_fd[1]) < 0) {
				tst_brkm(TBROK | TERRNO, cleanup,
					 "self_exec() failed");
			}
#else
			(void)child1_rout();
#endif
		} else {
			 * Fork failed.
			 */
			tst_brkm(TBROK | TERRNO, cleanup, "fork failed");
		}
	}
	cleanup();
	tst_exit();
void parent_rout(void)
{
	 *  Set to catch the alarm signal SIGALRM.
	 */
	if (signal(SIGALRM, notify_timeout) == SIG_ERR) {
		(void)par_kill();
		tst_brkm(TBROK, NULL,
			 "Could not set to catch the parents time out alarm.");
	}
	 *  Setting to catch the timeout alarm worked now let the children start up.
	 *  Set an alarm which causes a time out on the read pipe loop.
	 *  The children will notify the parent that set up is complete
	 *  and the pass/fail status of set up.
	 */
	(void)alarm(TIMEOUT);
	while ((read(pipe1_fd[0], pipe_buf, 1) != 1) && (alarm_flag == FALSE))
	strncpy(buf_tmp1, pipe_buf, 1);
	(void)alarm(TIMEOUT);
	while ((read(pipe2_fd[0], pipe_buf, 1) != 1) && (alarm_flag == FALSE))
	strncpy(buf_tmp2, pipe_buf, 1);
	 *  Check the alarm flag.
	 */
	if (alarm_flag == TRUE) {
		tst_brkm(TBROK, NULL,
			 "The set up of the children failed by timing out.");
		(void)par_kill();
		cleanup();
	}
	 *  Check to see if either child failed in the set up.
	 */
	if ((strncmp(buf_tmp1, CHAR_SET_FAILED, 1) == 0) ||
	    (strncmp(buf_tmp2, CHAR_SET_FAILED, 1) == 0)) {
		 * Problems were encountered in the set up of one of the children.
		 * The error message has been displayed by the child.
		 */
		(void)par_kill();
		cleanup();
	}
	 *  Setup passed, now send SIGUSR1 to process id of zero.
	 */
kill(0, SIGUSR1);
	if (TEST_RETURN == -1) {
		tst_brkm(TBROK | TTERRNO, NULL, "kill() failed");
		(void)par_kill();
		cleanup();
	}
	 *  Sleep for a while to allow the children to get a chance to
	 *  catch the signal.
	 */
	(void)sleep(SLEEP_TIME);
	 *  The signal was sent above and time has run out for child response,
	 *  check the outcomes.
	 */
	read1_stat = read(pipe1_fd[0], pipe_buf, 1);
	if (read1_stat == -1 && errno == EAGAIN)
		read1_stat = 0;
	read2_stat = read(pipe2_fd[0], pipe_buf, 1);
	if (read2_stat == -1 && errno == EAGAIN)
		read2_stat = 0;
	readA_stat = read(pipeA_fd[0], pipe_buf, 1);
	if (readA_stat == -1 && errno == EAGAIN)
		readA_stat = 0;
	readB_stat = read(pipeB_fd[0], pipe_buf, 1);
	if (readB_stat == -1 && errno == EAGAIN)
		readB_stat = 0;
	if (read1_stat == -1 || read2_stat == -1 ||
	    readA_stat == -1 || readB_stat == -1) {
		 * The read system call failed.
		 */
		tst_brkm(TBROK | TERRNO, NULL, "read() failed");
		(void)par_kill();
		cleanup();
	}
	 * Check the processes that were supposed to get the signal.
	 */
	if (read1_stat == SIG_RECEIVED) {
		if (readA_stat == SIG_RECEIVED) {
			 *  Both processes, 1 and A, that were supposed to receive
			 *  the signal did receive the signal.
			 */
			tst_resm(TPASS,
				 "The signal was sent to all processes in the process group.");
			tst_resm(TFAIL,
				 "Process A did not receive the signal.");
		}
		tst_resm(TFAIL, "Process 1 did not receive the signal.");
	}
	 * Check the processes that were not supposed to get the signal.
	 */
	if (read2_stat == SIG_NOT_RECD) {
		if (readB_stat == SIG_NOT_RECD) {
			 *  Both processes, 2 and B did not receive the signal.
			 */
			tst_resm(TPASS,
				 "The signal was not sent to selective processes that were not in the process group.");
			tst_resm(TFAIL, "Process B received the signal.");
		}
	}
		tst_resm(TFAIL, "Process 2 received the signal.");
	}
	(void)par_kill();
	(void)alarm(TIMEOUT);
	while ((read(pipe1_fd[0], pipe_buf, 1) != 1) && (alarm_flag == FALSE))
		strncpy(buf_tmp1, pipe_buf, 1);
void child1_rout(void)
{
	who_am_i = '1';
	 *  Set to catch the SIGUSR1 with int1_rout.
	 */
	if (signal(SIGUSR1, usr1_rout) == SIG_ERR) {
		tst_brkm(TBROK, NULL,
			 "Could not set to catch the childrens signal.");
		(void)write(pipe1_fd[1], CHAR_SET_FAILED, 1);
		exit(0);
	}
	 *  Create children A & B.
	 */
	if ((pidA = FORK_OR_VFORK()) > 0) {
		 *  This is the parent(child1), fork again to create child B.
		 */
		if ((pidB = FORK_OR_VFORK()) == 0) {
#ifdef UCLINUX
			if (self_exec(argv0, "nd", 2, pipeB_fd[1]) < 0) {
				tst_brkm(TBROK | TERRNO, NULL,
					 "self_exec() failed");
				(void)write(pipe1_fd[1], CHAR_SET_FAILED, 1);
				exit(0);
			}
#else
			(void)childB_rout();
#endif
		}
		else if (pidB == -1) {
			 *  The fork of child B failed kill child A.
			 */
			if (kill(pidA, SIGKILL) == -1)
				tst_resm(TWARN,
					 "Child process may not have been killed.");
			tst_brkm(TBROK | TERRNO, NULL, "fork failed");
			(void)write(pipe2_fd[1], CHAR_SET_FAILED, 1);
			exit(0);
		}
	}
	else if (pidA == 0) {
#ifdef UCLINUX
		if (self_exec(argv0, "nd", 1, pipeA_fd[1]) < 0) {
			tst_brkm(TBROK | TERRNO, NULL, "self_exec() failed");
			(void)write(pipe1_fd[1], CHAR_SET_FAILED, 1);
			exit(0);
		}
#else
		(void)childA_rout();
#endif
	}
	else if (pidA == -1) {
		 *  The fork of child A failed.
		 */
		tst_brkm(TBROK | TERRNO, NULL, "fork failed");
		(void)write(pipe1_fd[1], CHAR_SET_FAILED, 1);
		exit(0);
	}
	 *  Set to catch the SIGUSR2 with chld1_kill.
	 */
	if (signal(SIGUSR2, chld1_kill) == SIG_ERR) {
		tst_brkm(TBROK, NULL,
			 "Could not set to catch the parents signal.");
		(void)write(pipe1_fd[1], CHAR_SET_FAILED, 1);
		(void)chld1_kill();
		exit(0);
	}
	 *  Set to catch the alarm signal SIGALRM.
	 */
	if (signal(SIGALRM, notify_timeout) == SIG_ERR) {
		tst_brkm(TBROK, NULL,
			 "Could not set to catch the childs time out alarm.");
		(void)write(pipe1_fd[1], CHAR_SET_FAILED, 1);
		(void)chld1_kill();
		exit(0);
	}
	 *  Setting to catch the signals worked now let the children start up.
	 *  Set an alarm which causes a time out on the pipe read loop.
	 *  The children A & B will notify the parent(child1) that set up is complete
	 *  and the pass/fail status of set up.
	 */
	(void)alarm(TIMEOUT - 40);
	while ((read(pipeA_fd[0], pipe_buf, 1) != 1) && (alarm_flag == FALSE))
	(void)alarm(TIMEOUT - 40);
	while ((read(pipeB_fd[0], pipe_buf, 1) != 1) && (alarm_flag == FALSE))
	 *  Check the alarm flag.
	 */
	if (alarm_flag == TRUE) {
		tst_brkm(TBROK, NULL,
			 "The set up of the children failed by timing out.");
		(void)chld1_kill();
		(void)write(pipe1_fd[1], CHAR_SET_FAILED, 1);
		exit(0);
	}
	 *  Send a set up complete message to the parent.
	 */
	(void)write(pipe1_fd[1], CHAR_SET_PASSED, 1);
	 *  Pause until the signal SIGUSR1 or SIGUSR2 is sent from the parent.
	 */
	(void)pause();
	 *  Pause until signal SIGUSR2 is sent from the parent.
	 *  This pause will only be executed if SIGUSR2 has not been received yet.
	 */
	while (1) {
		sleep(1);
	}
void child2_rout(void)
{
	who_am_i = '2';
	 * Set the process group of this process to be different
	 * than the other processes.
	 */
	(void)setpgrp();
	 *  Set to catch the SIGUSR1 with usr1_rout.
	 */
	if (signal(SIGUSR1, usr1_rout) == SIG_ERR) {
		tst_brkm(TBROK, cleanup,
			 "Could not set to catch the parents signal.");
		(void)write(pipe2_fd[1], CHAR_SET_FAILED, 1);
		exit(0);
	}
	(void)write(pipe2_fd[1], CHAR_SET_PASSED, 1);
	 *  Pause until killed by the parent or SIGUSR1 is received.
	 */
	(void)pause();
}
void childA_rout(void)
{
	who_am_i = 'A';
	write(pipeA_fd[1], CHAR_SET_PASSED, 1);
	 *  Pause until killed by the parent or SIGUSR1 is received.
	 */
	(void)pause();
	exit(0);
#ifdef UCLINUX
void childA_rout_uclinux(void)
{
	if (signal(SIGUSR1, usr1_rout) == SIG_ERR) {
		tst_brkm(TBROK, NULL,
			 "Could not set to catch the childrens signal.");
		(void)write(pipeA_fd[1], CHAR_SET_FAILED, 1);
		exit(0);
	}
	childA_rout();
}
#endif
void childB_rout(void)
{
	who_am_i = 'B';
	 * Set the process group of this process to be different
	 * than the other processes.
	 */
	(void)setpgrp();
	write(pipeB_fd[1], CHAR_SET_PASSED, 1);
	 *  Pause until killed by the parent(child 1) or SIGUSR1 is received.
	 */
	(void)pause();
	exit(0);
}
#ifdef UCLINUX
void childB_rout_uclinux(void)
{
	if (signal(SIGUSR1, usr1_rout) == SIG_ERR) {
		tst_brkm(TBROK, NULL,
			 "Could not set to catch the childrens signal.");
		(void)write(pipeB_fd[1], CHAR_SET_FAILED, 1);
		exit(0);
	}
	childB_rout();
}
#endif
void setup(void)
{
	 *  Set the process group ID to be equal between the parent and children.
	 */
	(void)setpgrp();
	 *  Set to catch unexpected signals.
	 *  SIGCHLD is set to be ignored because we do not wait for termination status.
	 *  SIGUSR1 is set to be ignored because this is the signal we are using for
	 *  the test and we are not concerned with the parent getting it.
	 */
	tst_sig(FORK, DEF_HANDLER, cleanup);
	if (signal(SIGUSR1, SIG_IGN) == SIG_ERR) {
		tst_brkm(TBROK | TFAIL, NULL,
			 "signal(SIGUSR1, SIG_IGN) failed");
	}
	if (signal(SIGCHLD, SIG_IGN) == SIG_ERR) {
		tst_brkm(TBROK | TERRNO, NULL,
			 "signal(SIGCHLD, SIG_IGN) failed");
	}
	TEST_PAUSE;
	 *  Set up pipe1, pipe2, pipeA, and pipeB.
	 */
	if ((pipe(pipe1_fd) == -1)
	    || (fcntl(pipe1_fd[0], F_SETFL, O_NDELAY) == -1)) {
		errno_buf = errno;
		err_flag = TRUE;
	}
	if ((pipe(pipe2_fd) == -1)
	    || (fcntl(pipe2_fd[0], F_SETFL, O_NDELAY) == -1)) {
		errno_buf = errno;
		err_flag = TRUE;
	}
	if ((pipe(pipeA_fd) == -1)
	    || (fcntl(pipeA_fd[0], F_SETFL, O_NDELAY) == -1)) {
		errno_buf = errno;
		err_flag = TRUE;
	}
	if ((pipe(pipeB_fd) == -1)
	    || (fcntl(pipeB_fd[0], F_SETFL, O_NDELAY) == -1)) {
		errno_buf = errno;
		err_flag = TRUE;
	}
	 *  Check for errors.
	 */
	if (err_flag == TRUE) {
		tst_brkm(TBROK | TERRNO, NULL, "pipe() failed");
	}
	return;
}
void usr1_rout(void)
{
	switch (who_am_i) {
	case '1':
		if (write(pipe1_fd[1], SIG_CAUGHT, 1) == -1)
			tst_resm(TWARN,
				 "Writing signal catching status failed in child 1.");
		break;
	case '2':
		if (write(pipe2_fd[1], SIG_CAUGHT, 1) == -1)
			tst_resm(TWARN,
				 "Writing signal catching status failed in child 2.");
		break;
	case 'A':
		if (write(pipeA_fd[1], SIG_CAUGHT, 1) == -1)
			tst_resm(TWARN,
				 "Writing signal catching status failed in child A.");
		break;
	case 'B':
		if (write(pipeB_fd[1], SIG_CAUGHT, 1) == -1)
			tst_resm(TWARN,
				 "Writing signal catching status failed in child B.");
		break;
	default:
		tst_resm(TWARN,
			 "Unexpected value %d for who_am_i in usr1_rout()",
			 who_am_i);
		break;
	}
void notify_timeout(void)
{
	alarm_flag = TRUE;
void par_kill(void)
{
	int status;
	 *  Indicate to child1 that it can remove it's children and itself now.
	 */
	if (kill(pid1, SIGUSR2) == -1 && errno != ESRCH) {
		tst_resm(TWARN | TERRNO, "kill() failed");
		tst_resm(TWARN,
			 "Child 1 and it's children may still be alive.");
	}
	 *  Remove child 2.
	 */
	if (kill(pid2, SIGKILL) == -1 && errno != ESRCH)
		tst_resm(TWARN, "Child2 may still be alive.");
	wait(&status);
	return;
void chld1_kill(void)
{
	 *  Remove children A & B.
	 */
	if (kill(pidA, SIGKILL) == -1 && errno != ESRCH)
		tst_resm(TWARN | TERRNO,
			 "kill(%d) failed; child 1's(A) child may still be alive",
			 pidA);
	(void)write(pipe1_fd[1], CHAR_SET_PASSED, 1);
	if (kill(pidB, SIGKILL) == -1 && errno != ESRCH)
		tst_resm(TWARN | TERRNO,
			 "kill(%d) failed; child 1's(B) child may still be alive",
			 pidB);
	exit(0);
void cleanup(void)
{
}

