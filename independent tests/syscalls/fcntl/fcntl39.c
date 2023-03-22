#include "incl.h"
#define	TEST_DIR	"test_dir"
#define	TEST_DIR2	"test_dir2"
#define	TEST_FILE	"test_file"
#define TEST_SIG (SIGRTMIN+1)

int parent_fd, subdir_fd;

int got_parent_event, got_subdir_event;

void dnotify_handler(int sig, siginfo_t *si, void *data LTP_ATTRIBUTE_UNUSED)
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
fcntl(fd, F_NOTIFY, DN_RENAME|DN_MULTISHOT);
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
	tst_res(TINFO, "Testing no DN_RENAME on rename from parent to subdir");
	rename(TEST_FILE, TEST_DIR "/" TEST_FILE);
	if (got_parent_event)
		tst_res(TFAIL, "Got unexpected event on parent");
	else
		tst_res(TPASS, "No event on parent as expected");
	if (got_subdir_event)
		tst_res(TFAIL, "Got unexpected event on subdir");
	else
		tst_res(TPASS, "No event on subdir as expected");
	tst_res(TINFO, "Testing DN_RENAME on rename of subdir itself");
	rename(TEST_DIR, TEST_DIR2);
	if (got_parent_event)
		tst_res(TPASS, "Got event on parent as expected");
	else
		tst_res(TFAIL, "Missing event on parent");
	if (got_subdir_event)
		tst_res(TFAIL, "Got unexpected event on subdir");
	else
		tst_res(TPASS, "No event on subdir as expected");
	close(parent_fd);
	close(subdir_fd);
	rename(TEST_DIR2 "/" TEST_FILE, TEST_FILE);
	rename(TEST_DIR2, TEST_DIR);
	got_parent_event = 0;
	got_subdir_event = 0;
}

void setup(void)
{
	mkdir(TEST_DIR, 00700);
	touch(TEST_FILE, 0666, NULL);
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
