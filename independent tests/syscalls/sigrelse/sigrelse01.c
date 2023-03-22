#include "incl.h"
#ifdef __linux__
extern int sighold(int __sig);
extern int sigrelse(int __sig);
#endif
#define SIGCANCEL 32
#define SIGTIMER 33
void setup(void);
void cleanup(void);

void parent(void);

void child(void);

void timeout(int sig);

int setup_sigs(void);

void handler(int sig);

void wait_a_while(void);

char *read_pipe(int fd);

int write_pipe(int fd, char *msg);

int set_timeout(void);

void clear_timeout(void);

void getout(void);
int choose_sig(int sig);
#define TRUE  1
#define FALSE 0
#ifndef DEBUG
#define DEBUG 0
#endif
#define EXIT_OK    0
#define SIG_CAUGHT 8
#define WRITE_BROK 16
#define HANDLE_ERR 32






#ifndef NUMSIGS
#define NUMSIGS NSIG
#endif

int sig_array[NUMSIGS];
int main(int argc, char **argv)
{
	int lc;
	   external declarations. */
	sig_caught = FALSE;
	 * parse standard options
	 */
	tst_parse_opts(argc, argv, NULL, NULL);
#ifdef UCLINUX
	maybe_run_child(&child, "dd", &pipe_fd[1], &pipe_fd2[0]);
#endif
	 * perform global setup for test
	 */
	setup();
	for (lc = 0; TEST_LOOPING(lc); lc++) {
		tst_count = 0;
		 * fork off a child process
		 */
		if ((pid = FORK_OR_VFORK()) < 0) {
			tst_brkm(TBROK | TERRNO, cleanup, "fork() failed");
		} else if (pid > 0) {
			parent();
		} else {
#ifdef UCLINUX
			if (self_exec(argv[0], "dd", pipe_fd[1], pipe_fd2[0]) <
			    0) {
				tst_brkm(TBROK | TERRNO, cleanup,
					 "self_exec() failed");
			}
#else
			child();
#endif
		}
	}
	cleanup();
	tst_exit();

void parent(void)
{
	int caught_sigs;
	if ((str = read_pipe(pipe_fd[0])) == NULL) {
		tst_brkm(TBROK, getout, "%s", mesg);
	}
	if (strcmp(str, READY) != 0) {
		tst_brkm(TBROK, getout, "%s", str);
	}
	 * send signals to child and see if it holds them
	 */
	for (sig = 1; sig < NUMSIGS; sig++) {
		if (choose_sig(sig)) {
			if (kill(pid, sig) < 0) {
				if (errno == ESRCH) {
					if (kill(pid, SIGTERM) < 0)
						tst_brkm(TBROK | TERRNO, getout,
							 "kill(%d, %d) and kill(%d, SIGTERM) failed",
							 pid, sig, pid);
					else
						tst_brkm(TBROK | TERRNO, getout,
							 "kill(%d, %d) failed, but kill(%d, SIGTERM) worked",
							 pid, sig, pid);
				} else
					tst_brkm(TBROK | TERRNO, getout,
						 "kill(%d, %d) failed", pid,
						 sig);
			}
		}
	}
	if (write_pipe(pipe_fd2[1], READY) < 0) {
		tst_brkm(TBROK | TERRNO, getout,
			 "Unable to tell child to go, write to pipe failed");
	}
	 * child is now releasing signals, wait and check exit value
	 */
	wait(getout, &term_stat);
	if ((sig = CHILD_SIG(term_stat)) != 0)
		tst_brkm(TBROK, cleanup, "Unexpected signal %d killed child",
			 sig);
	rv = CHILD_EXIT(term_stat);
	switch (rv) {
	case EXIT_OK:
		if ((array = (int *)read_pipe(pipe_fd[0])) == NULL) {
			tst_resm(TBROK, "%s", mesg);
			break;
		}
#if DEBUG > 1
		for (sig = 1; sig < NUMSIGS; sig++) {
			printf("array[%d] = %d\n", sig, array[sig]);
		}
#endif
		caught_sigs = 0;
		for (sig = 1; sig < NUMSIGS; sig++) {
			if (choose_sig(sig)) {
				if (array[sig] != 1) {
					(void)sprintf(mesg,
						      "\tsignal %d caught %d times (expected 1).\n",
						      sig, array[sig]);
					(void)strcat(big_mesg, mesg);
					fail = TRUE;
				} else {
					caught_sigs++;
				}
			}
		if (fail == TRUE)
			tst_resm(TFAIL, "%s", big_mesg);
		else
			tst_resm(TPASS,
				 "sigrelse() released all %d signals under test.",
				 caught_sigs);
		break;
	case TBROK:
		if ((str = read_pipe(pipe_fd[0])) == NULL) {
			tst_resm(TBROK, "%s", mesg);
			break;
		}
		tst_resm(TBROK, "%s", str);
		break;
	case SIG_CAUGHT:
		tst_resm(TBROK, "A signal was caught before being released.");
		break;
	case WRITE_BROK:
		tst_resm(TBROK, "write() pipe failed for child.");
		break;
	case HANDLE_ERR:
		tst_resm(TBROK, "Error occured in signal handler.");
		break;
	default:
		tst_resm(TBROK, "Unexpected exit code %d from child", rv);
		break;
	}

void child(void)
{
	char *str;
	(void)strcpy(note, READY);
	if (set_timeout() < 0) {
		(void)strcpy(note, mesg);
	} else if (setup_sigs() < 0) {
		(void)strcpy(note, mesg);
	} else {
		for (sig = 1; sig < NUMSIGS; sig++) {
			if (choose_sig(sig)) {
				if ((rv = sighold(sig)) != 0) {
					(void)sprintf(note,
						      "sighold did not return 0. rv:%d",
						      rv);
					break;
				}
			}
		}
	}
	 * send note to parent (if not READY, parent will BROK) and
	 * wait for parent to send signals.  The timeout clock is set so
	 * that we will not wait forever - if sighold() did its job, we
	 * will not receive the signals.  If sighold() blew it we will
	 * catch a signal and the interrupt handler will exit with a
	 * value of SIG_CAUGHT.
	 */
	if (write_pipe(pipe_fd[1], note) < 0) {
		 * write_pipe() failed.  Set exit value to WRITE_BROK to let
		 * parent know what happened
		 */
		clear_timeout();
		exit(WRITE_BROK);
	}
	 * if we get to this point, all signals have been held and the
	 * timer has expired.  Now what we want to do is release each
	 * signal and see if we catch it.  If we catch all signals,
	 * sigrelse passed, else it failed.
	 */
#if DEBUG > 0
	printf("child: PHASE II\n");
#endif
	exit_val = EXIT_OK;
#if DEBUG > 0
	printf("child: pid=%d waiting for parent's ready...\n", getpid());
#endif
	 * wait for parent to tell us that sigals were all sent
	 */
	if ((str = read_pipe(pipe_fd2[0])) == NULL) {
		printf(" child: read_pipe failed\n");
		exit(TBROK);
	}
	if (strcmp(str, READY) != 0) {
		printf("child: didn't proper ready message\n");
		exit(TBROK);
	}
	for (sig = 1; sig < NUMSIGS; sig++) {
		if (choose_sig(sig)) {
#if DEBUG > 1
			printf("child: releasing sig %d...\n", sig);
#endif
			if ((rv = sigrelse(sig)) != 0) {
				(void)sprintf(note,
					      "sigrelse did not return 0. rv:%d",
					      rv);
				exit_val = TBROK;
				break;
			}
			wait_a_while();
		}
	 * If we are error free so far...
	 * check the sig_array array for one occurence of
	 * each of the catchable signals.  If this is true,
	 * then PASS, otherwise FAIL.
	 */
	if (exit_val == EXIT_OK) {
		(void)memcpy(note, (char *)sig_array, sizeof(sig_array));
	}
	if (write_pipe(pipe_fd[1], note) < 0) {
		 * write_pipe() failed.  Set exit value to WRITE_BROK to let
		 * parent know what happened
		 */
		exit(WRITE_BROK);
	}
	exit(exit_val);

int setup_sigs(void)
{
	int sig;
	for (sig = 1; sig < NUMSIGS; sig++) {
		if (choose_sig(sig)) {
			if (signal(sig, handler) == SIG_ERR) {
				(void)sprintf(mesg,
					      "signal() failed for signal %d. error:%d %s.",
					      sig, errno, strerror(errno));
				return (-1);
			}
		}
	}
	return 0;

void handler(int sig)
{
	

#if DEBUG > 1
	printf("child: handler phase%d: caught signal %d.\n", phase, sig);
#endif
	if (phase == 1) {
		exit(SIG_CAUGHT);
	} else {
		++s;
		if (s > 1) {
			exit(HANDLE_ERR);
		}
		++sig_array[sig];
		--s;
	}
	return;

char *read_pipe(int fd)
{
	

	int ret;
#if DEBUG > 0
	printf("read_pipe: pid=%d waiting...\n", getpid());
#endif
	if (set_timeout() < 0) {
		return NULL;
	}
	ret = -1;
		if ((ret = read(fd, buf, MAXMESG)) == 0) {
			(void)sprintf(mesg, "read() pipe failed. error:%d %s.",
				      errno, strerror(errno));
			clear_timeout();
			return NULL;
		}
	}
	clear_timeout();
#if DEBUG > 0
	printf("read_pipe: pid=%d received: %s.\n", getpid(), buf);
#endif
	return (buf);

int write_pipe(int fd, char *msg)
{
#if DEBUG > 0
	printf("write_pipe: pid=%d, sending %s.\n", getpid(), msg);
#endif
	if (write(fd, msg, MAXMESG) < 0) {
		(void)sprintf(mesg, "write() pipe failed. error:%d %s.",
			      errno, strerror(errno));
		return (-1);
	}
	return 0;

int set_timeout(void)
{
	if (signal(SIGALRM, timeout) == SIG_ERR) {
		(void)sprintf(mesg,
			      "signal() failed for signal %d. error:%d %s.",
			      SIGALRM, errno, strerror(errno));
		return (-1);
	}
	(void)alarm(TIMEOUT);
	return 0;

void clear_timeout(void)
{
	(void)alarm(0);

void timeout(int sig)
{
#if DEBUG > 0
	printf("timeout: pid=%d sigalrm caught.\n", getpid());
#endif
}

void wait_a_while(void)
{
	long btime;
	btime = time(NULL);
	while (time(NULL) - btime < TIMEOUT) {
		if (sig_caught == TRUE)
			break;
	}

void getout(void)
{
	if (pid > 0 && kill(pid, SIGKILL) < 0)
		tst_resm(TWARN, "kill(%d, SIGKILL) failed", pid);
	cleanup();
#ifdef VAX

int sighold(int signo)
{
	return 0;
}

int sigrelse(signo)
int signo;
{
	return 0;
}
#endif
int choose_sig(int sig)
{
	switch (sig) {
	case SIGKILL:
	case SIGSTOP:
	case SIGTSTP:
	case SIGCONT:
	case SIGALRM:
	case SIGCANCEL:
	case SIGTIMER:
#ifdef SIGNOBDM
	case SIGNOBDM:
#endif
#ifdef SIGTTIN
	case SIGTTIN:
#endif
#ifdef SIGTTOU
	case SIGTTOU:
#endif
#ifdef  SIGPTINTR
	case SIGPTINTR:
#endif
#ifdef  SIGSWAP
	case SIGSWAP:
#endif
		return 0;
	}
	return 1;
}
void setup(void)
{
	tst_sig(FORK, DEF_HANDLER, cleanup);
	TEST_PAUSE;
	tst_tmpdir();
	pipe(cleanup, pipe_fd);
	 * Cause the read to return 0 once EOF is encountered and the
	 * read to return -1 if pipe is empty.
	 */
	if (fcntl(pipe_fd[0], F_SETFL, O_NONBLOCK) == -1)
		tst_brkm(TBROK | TERRNO, cleanup,
			 "fcntl(Fds[0], F_SETFL, O_NONBLOCK) failed");
	pipe(cleanup, pipe_fd2);
	 * Cause the read to return 0 once EOF is encountered and the
	 * read to return -1 if pipe is empty.
	 */
	if (fcntl(pipe_fd2[0], F_SETFL, O_NONBLOCK) == -1)
		tst_brkm(TBROK | TERRNO, cleanup,
			 "fcntl(Fds[0], F_SETFL, O_NONBLOCK) failed");
}
void cleanup(void)
{
	tst_rmdir();
}

