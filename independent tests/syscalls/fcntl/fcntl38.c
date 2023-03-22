#include "incl.h"
#define	TEST_DIR	"test_dir"
#define TEST_SIG SIGRTMIN+1

int parent_fd, subdir_fd;

int got_parent_event, got_subdir_event;

void dnotify_handler(int sig, siginfo_t *si, void *data __attribute__((unused)))
{
	if (si->si_fd == parent_fd)
		got_parent_event = 1;
	else if (si->si_fd == subdir_fd)
		got_subdir_event = 1;
	else
		tst_brk(TBROK, "Got unexpected signal %d with si_fd %d", sig, si->si_fd);
}

void setup_dnotify(int fd)
{
	struct sigaction act;
	act.sa_sigaction = dnotify_handler;
	sigemptyset(&act.sa_mask);
	act.sa_flags = SA_SIGINFO;
	sigaction(TEST_SIG, &act, NULL);
fcntl(fd, F_SETSIG, TEST_SIG);
	if (TST_RET != 0) {
		tst_brk(TBROK, "F_SETSIG failed errno = %d : %s",
			TST_ERR, strerror(TST_ERR));
	}
fcntl(fd, F_NOTIFY, DN_ATTRIB|DN_MULTISHOT);
	if (TST_RET != 0) {
		tst_brk(TBROK, "F_NOTIFY failed errno = %d : %s",
			TST_ERR, strerror(TST_ERR));
	}
}

int  verify_dnotify(
{
	parent_fd = open(".", O_RDONLY);
	subdir_fd = open(TEST_DIR, O_RDONLY);
	setup_dnotify(parent_fd);
	setup_dnotify(subdir_fd);
	chmod(TEST_DIR, 0755);
	if (got_parent_event)
		tst_res(TPASS, "Got event on parent as expected");
	else
		tst_res(TFAIL, "Missing event on parent");
	if (got_subdir_event)
		tst_res(TPASS, "Got event on subdir as expected");
	else
		tst_res(TFAIL, "Missing event on subdir");
	close(parent_fd);
	close(subdir_fd);
}

void setup(void)
{
	mkdir(TEST_DIR, 00700);
}

void cleanup(void)
{
	if (parent_fd > 0)
		close(parent_fd);
	if (subdir_fd > 0)
		close(subdir_fd);
}

void main(){
	setup();
	cleanup();
}
