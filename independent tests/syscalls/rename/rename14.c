#include "incl.h"
#define FAILED 0
#define PASSED 1
int local_flag = PASSED;
char *TCID = "rename14";
int TST_TOTAL = 1;
#define RUNTIME	5
int kidpid[2];
int parent_pid;
int term(void);
int al(void);
void dochild1(void);
void dochild2(void);
int main(int argc, char *argv[])
{
	int pid;
	sigset_t set;
	struct sigaction act, oact;
	tst_parse_opts(argc, argv, NULL, NULL);
	sigemptyset(&set);
	act.sa_handler = (void (*)())term;
	act.sa_mask = set;
	act.sa_flags = 0;
	if (sigaction(SIGTERM, &act, &oact)) {
		tst_brkm(TBROK, NULL, "Sigaction(SIGTERM)");
	}
	sigemptyset(&set);
	act.sa_handler = (void (*)())al;
	act.sa_mask = set;
	act.sa_flags = 0;
	if (sigaction(SIGALRM, &act, 0)) {
		tst_brkm(TBROK, NULL, "Sigaction(SIGALRM)");
	}
	parent_pid = getpid();
	tst_tmpdir();
	pid = FORK_OR_VFORK();
	if (pid < 0)
		tst_brkm(TBROK, NULL, "fork() returned %d", pid);
	if (pid == 0)
		dochild1();
	kidpid[0] = pid;
	pid = FORK_OR_VFORK();
	if (pid < 0) {
		(void)kill(kidpid[0], SIGTERM);
		(void)unlink("./rename14");
		tst_brkm(TBROK, NULL, "fork() returned %d", pid);
	}
	if (pid == 0)
		dochild2();
	kidpid[1] = pid;
	alarm(RUNTIME);
	pause();
	kill(kidpid[0], SIGTERM);
	kill(kidpid[1], SIGTERM);
	waitpid(kidpid[0], NULL, 0);
	waitpid(kidpid[1], NULL, 0);
	unlink("./rename14");
	unlink("./rename14xyz");
	(local_flag == PASSED) ? tst_resm(TPASS, "Test Passed")
	    : tst_resm(TFAIL, "Test Failed");
	tst_rmdir();
	tst_exit();
}
int term(void)
{
	if (parent_pid != getpid())
		exit(0);
	if (kidpid[0])
		return (kill(kidpid[0], SIGTERM));
	if (kidpid[1])
		return (kill(kidpid[1], SIGTERM));
	return 0;
}
int al(void)
{
	if (kidpid[0])
		return (kill(kidpid[0], SIGTERM));
	if (kidpid[1])
		return (kill(kidpid[1], SIGTERM));
	return 0;
}
void dochild1(void)
{
	int fd;
	for (;;) {
		fd = creat("./rename14", 0666);
		unlink("./rename14");
		close(fd);
	}
}
void dochild2(void)
{
	for (;;)
		rename("./rename14", "./rename14xyz");
}

