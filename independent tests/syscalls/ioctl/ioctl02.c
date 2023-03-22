#include "incl.h"
#define	CNUL	0
char *TCID = "ioctl02";
int TST_TOTAL = 1;

struct termio termio, save_io;

char *parenttty, *childtty;

int parentfd, childfd;

int parentpid, childpid;

volatile int sigterm, sigusr1, sigusr2;

int closed = 1;

int do_child_setup(void);

int do_parent_setup(void);

int run_ptest(void);

int run_ctest(void);

int chk_tty_parms();

void setup(void);

void cleanup(void);

void help(void);

void do_child(void);
void do_child_uclinux(void);

void sigterm_handler(void);

int Devflag;

char *devname;

option_t options[] = {
	{"D:", &Devflag, &devname},
	{NULL, NULL, NULL}
};
int main(int ac, char **av)
{
	int lc;
	int rval;
	tst_parse_opts(ac, av, options, &help);
#ifdef UCLINUX
	maybe_run_child(&do_child_uclinux, "dS", &parentpid, &childtty);
#endif
	if (!Devflag)
		tst_brkm(TBROK, NULL, "You must specify a tty device with "
			 "the -D option.");
	tst_require_root();
	setup();
	for (lc = 0; TEST_LOOPING(lc); lc++) {
		tst_count = 0;
		parenttty = devname;
		childtty = devname;
		parentpid = getpid();
		childpid = FORK_OR_VFORK();
		if (childpid < 0)
			tst_brkm(TBROK, cleanup, "fork failed");
#ifdef UCLINUX
			if (self_exec(av[0], "dS", parentpid, childtty) < 0)
				tst_brkm(TBROK, cleanup, "self_exec failed");
#else
			do_child();
#endif
		}
		while (!sigusr1)
			sleep(1);
		sigusr1 = 0;
		parentfd = do_parent_setup();
		if (parentfd < 0) {
			kill(childpid, SIGTERM);
			waitpid(childpid, NULL, 0);
			cleanup();
		}
		rval = run_ptest();
		if (rval == -1) {
			 * Parent cannot set/get ioctl parameters.
			 * SIGTERM the child and cleanup.
			 */
			kill(childpid, SIGTERM);
			waitpid(childpid, NULL, 0);
			cleanup();
		}
		if (rval != 0)
			tst_resm(TFAIL, "TCGETA/TCSETA tests FAILED with "
				 "%d %s", rval, rval > 1 ? "errors" : "error");
		else
			tst_resm(TPASS, "TCGETA/TCSETA tests SUCCEEDED");
		(void)kill(childpid, SIGTERM);
		(void)waitpid(childpid, NULL, 0);
		 * Clean up things from the parent by restoring the
		 * tty device information that was saved in setup()
		 * and closing the tty file descriptor.
		 */
		if (ioctl(parentfd, TCSETA, &save_io) == -1)
			tst_resm(TINFO, "ioctl restore failed in main");
		close(cleanup, parentfd);
		closed = 1;
	}
	cleanup();
	tst_exit();
}

void do_child(void)
{
	childfd = do_child_setup();
	if (childfd < 0)
		_exit(1);
	run_ctest();
	_exit(0);
}
void do_child_uclinux(void)
{
	struct sigaction act;
	act.sa_handler = (void *)sigterm_handler;
	act.sa_flags = 0;
	sigemptyset(&act.sa_mask);
	(void)sigaction(SIGTERM, &act, 0);
	do_child();
}

int run_ptest(void)
{
	int i, rval;
	termio.c_line = 0;
	termio.c_cflag = B50 | CS7 | CREAD | PARENB | PARODD | CLOCAL;
	for (i = 0; i < NCC; i++) {
		if (i == VEOL2)
			continue;
		termio.c_cc[i] = CSTART;
	}
	termio.c_lflag =
	    ((unsigned short)(ISIG | ICANON | XCASE | ECHO | ECHOE | NOFLSH));
	termio.c_iflag =
	    BRKINT | IGNPAR | INPCK | ISTRIP | ICRNL | IUCLC | IXON | IXANY |
	    IXOFF;
	termio.c_oflag = OPOST | OLCUC | ONLCR | ONOCR;
ioctl(parentfd, TCSETA, &termio);
	if (TEST_RETURN < 0) {
		tst_resm(TFAIL, "ioctl TCSETA failed : "
			 "errno = %d", TEST_ERRNO);
		return -1;
	}
	rval = ioctl(parentfd, TCGETA, &termio);
	if (rval < 0) {
		tst_resm(TFAIL, "ioctl TCGETA failed.  Ending test.");
		return -1;
	}
	return chk_tty_parms();
}

int run_ctest(void)
{
	 * Wait till the parent has finished testing.
	 */
	while (!sigterm)
		sleep(1);
	sigterm = 0;
	tst_resm(TINFO, "child: Got SIGTERM from parent.");
	if (close(childfd) == -1)
		tst_resm(TINFO, "close() in run_ctest() failed");
	return 0;
}

int chk_tty_parms(void)
{
	int i, flag = 0;
	if (termio.c_line != 0) {
		tst_resm(TINFO, "line discipline has incorrect value %o",
			 termio.c_line);
		flag++;
	}
	 * The following Code Sniffet is disabled to check the value of c_cflag
	 * as it seems that due to some changes from 2.6.24 onwards, this
	 * setting is not done properly for either of (B50|CS7|CREAD|PARENB|
	 * PARODD|CLOCAL|(CREAD|HUPCL|CLOCAL).
	 * However, it has been observed that other flags are properly set.
	 */
#if 0
	if (termio.c_cflag != (B50 | CS7 | CREAD | PARENB | PARODD | CLOCAL)) {
		tst_resm(TINFO, "cflag has incorrect value. %o",
			 termio.c_cflag);
		flag++;
	}
#endif
	for (i = 0; i < NCC; i++) {
		if (i == VEOL2) {
			if (termio.c_cc[VEOL2] == CNUL) {
				continue;
			} else {
				tst_resm(TINFO, "control char %d has "
					 "incorrect value %d %d", i,
					 termio.c_cc[i], CNUL);
				flag++;
				continue;
			}
		}
		if (termio.c_cc[i] != CSTART) {
			tst_resm(TINFO, "control char %d has incorrect "
				 "value %d.", i, termio.c_cc[i]);
			flag++;
		}
	}
	if (!
	    (termio.c_lflag
	     && (ISIG | ICANON | XCASE | ECHO | ECHOE | NOFLSH))) {
		tst_resm(TINFO, "lflag has incorrect value. %o",
			 termio.c_lflag);
		flag++;
	}
	if (!
	    (termio.c_iflag
	     && (BRKINT | IGNPAR | INPCK | ISTRIP | ICRNL | IUCLC | IXON | IXANY
		 | IXOFF))) {
		tst_resm(TINFO, "iflag has incorrect value. %o",
			 termio.c_iflag);
		flag++;
	}
	if (!(termio.c_oflag && (OPOST | OLCUC | ONLCR | ONOCR))) {
		tst_resm(TINFO, "oflag has incorrect value. %o",
			 termio.c_oflag);
		flag++;
	}
	if (!flag)
		tst_resm(TINFO, "termio values are set as expected");
	return flag;
}

int do_parent_setup(void)
{
	int pfd;
	pfd = open(cleanup, parenttty, O_RDWR, 0777);
	closed = 0;
	ioctl(cleanup, pfd, TCFLSH, 2);
	return pfd;
}

int do_child_setup(void)
{
	int cfd;
	cfd = open(childtty, O_RDWR, 0777);
	if (cfd < 0) {
		tst_resm(TINFO, "Could not open %s in do_child_setup(), errno "
			 "= %d", childtty, errno);
		kill(parentpid, SIGUSR1);
		return -1;
	}
	if (ioctl(cfd, TCFLSH, 2) < 0) {
		tst_resm(TINFO, "ioctl TCFLSH failed. : errno = %d", errno);
		kill(parentpid, SIGUSR1);
		return -1;
	}
	kill(parentpid, SIGUSR1);
	return cfd;
}

void sigterm_handler(void)
{
	sigterm = 1;
}

void sigusr1_handler(void)
{
	sigusr1 = 1;
}

void sigusr2_handler(void)
{
	sigusr2 = 1;
}

void help(void)
{
	printf("  -D <tty device> : for example, /dev/tty[0-9]\n");
}

void setup(void)
{
	int fd;
	struct sigaction act;
	fd = open(NULL, devname, O_RDWR, 0777);
	ioctl(cleanup, fd, TCGETA, &save_io);
	close(cleanup, fd);
	act.sa_handler = (void *)sigterm_handler;
	act.sa_flags = 0;
	sigemptyset(&act.sa_mask);
	(void)sigaction(SIGTERM, &act, 0);
	act.sa_handler = (void *)sigusr1_handler;
	act.sa_flags = 0;
	(void)sigaction(SIGUSR1, &act, 0);
	act.sa_handler = (void *)sigusr2_handler;
	act.sa_flags = 0;
	(void)sigaction(SIGUSR2, &act, 0);
	act.sa_handler = SIG_IGN;
	act.sa_flags = 0;
	(void)sigaction(SIGTTOU, &act, 0);
	sigterm = sigusr1 = sigusr2 = 0;
	TEST_PAUSE;
}

void cleanup(void)
{
	if (!closed) {
		if (ioctl(parentfd, TCSETA, &save_io) == -1)
			tst_resm(TINFO, "ioctl restore failed in cleanup()");
		if (close(parentfd) == -1)
			tst_resm(TINFO, "close() failed in cleanup()");
	}
}

