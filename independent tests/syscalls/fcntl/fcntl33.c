#include "incl.h"
#define MIN_TIME_LIMIT	5
#define OP_OPEN_RDONLY	0
#define OP_OPEN_WRONLY	1
#define OP_OPEN_RDWR	2
#define OP_TRUNCATE	3
#define FILE_MODE	(S_IRWXU | S_IRWXG | S_IRWXO | S_ISUID | S_ISGID)
#define PATH_LS_BRK_T	"/proc/sys/fs/lease-break-time"

void do_test(unsigned int);

void do_child(unsigned int);

int fd;

int ls_brk_t;

long type;

sigset_t newset, oldset;

struct timespec timeout = {.tv_sec = 5};

struct test_case_t {
	int lease_type;
	int op_type;
	char *desc;
} test_cases[] = {
	{F_WRLCK, OP_OPEN_RDONLY,
		"open(O_RDONLY) conflicts with fcntl(F_SETLEASE, F_WRLCK)"},
	{F_WRLCK, OP_OPEN_WRONLY,
		"open(O_WRONLY) conflicts with fcntl(F_SETLEASE, F_WRLCK)"},
	{F_WRLCK, OP_OPEN_RDWR,
		"open(O_RDWR) conflicts with fcntl(F_SETLEASE, F_WRLCK)"},
	{F_WRLCK, OP_TRUNCATE,
		"truncate() conflicts with fcntl(F_SETLEASE, F_WRLCK)"},
	{F_RDLCK, OP_OPEN_WRONLY,
		"open(O_WRONLY) conflicts with fcntl(F_SETLEASE, F_RDLCK)"},
	{F_RDLCK, OP_OPEN_RDWR,
		"open(O_RDWR) conflicts with fcntl(F_SETLEASE, F_RDLCK)"},
	{F_RDLCK, OP_TRUNCATE,
		"truncate() conflicts with fcntl(F_SETLEASE, F_RDLCK)"},
};

void setup(void)
{
	tst_timer_check(CLOCK_MONOTONIC);
	file_scanf(PATH_LS_BRK_T, "%d", &ls_brk_t);
	fprintf(PATH_LS_BRK_T, "%d", 45);
	touch("file", FILE_MODE, NULL);
	sigemptyset(&newset);
	sigaddset(&newset, SIGIO);
	if (sigprocmask(SIG_SETMASK, &newset, &oldset) < 0)
		tst_brk(TBROK | TERRNO, "sigprocmask() failed");
}

void do_test(unsigned int i)
{
	pid_t cpid;
	cpid = fork();
	if (cpid == 0) {
		do_child(i);
		return;
	}
	fd = open("file", O_RDONLY);
fcntl(fd, F_SETLEASE, test_cases[i].lease_type);
	if (TST_RET == -1) {
		if (type == TST_OVERLAYFS_MAGIC && TST_ERR == EAGAIN) {
			tst_res(TINFO | TTERRNO,
				"fcntl(F_SETLEASE, F_WRLCK) failed on overlayfs as expected");
		} else {
			tst_res(TFAIL | TTERRNO, "fcntl() failed to set lease");
		}
		TST_CHECKPOINT_WAKE(0);
		goto exit;
	}
	TST_CHECKPOINT_WAKE(0);
sigtimedwait(&newset, NULL, &timeout);
	if (TST_RET == -1) {
		if (TST_ERR == EAGAIN) {
			tst_res(TFAIL | TTERRNO,
				"failed to receive SIGIO within %lis",
				timeout.tv_sec);
			goto exit;
		}
		tst_brk(TBROK | TTERRNO, "sigtimedwait() failed");
	}
	switch (test_cases[i].lease_type) {
	case F_WRLCK:
fcntl(fd, F_SETLEASE, F_RDLCK);
		if (TST_RET == 0) {
			if (test_cases[i].op_type == OP_OPEN_RDONLY)
				break;
			tst_res(TFAIL,
				"fcntl() downgraded lease when not read-only");
		}
		if (test_cases[i].op_type == OP_OPEN_RDONLY) {
			tst_res(TFAIL | TTERRNO,
				"fcntl() failed to downgrade lease");
		}
	case F_RDLCK:
fcntl(fd, F_SETLEASE, F_UNLCK);
		if (TST_RET == -1) {
			tst_res(TFAIL | TTERRNO,
				 "fcntl() failed to remove the lease");
		}
		break;
	default:
		break;
	}
exit:
	tst_reap_children();
	close(fd);
}

void do_child(unsigned int i)
{
	long long elapsed_ms;
	TST_CHECKPOINT_WAIT(0);
	tst_timer_start(CLOCK_MONOTONIC);
	switch (test_cases[i].op_type) {
	case OP_OPEN_RDONLY:
		open("file", O_RDONLY);
		break;
	case OP_OPEN_WRONLY:
		open("file", O_WRONLY);
		break;
	case OP_OPEN_RDWR:
		open("file", O_RDWR);
		break;
	case OP_TRUNCATE:
		truncate("file", 0);
		break;
	default:
		break;
	}
	tst_timer_stop();
	elapsed_ms = tst_timer_elapsed_ms();
	if (elapsed_ms < MIN_TIME_LIMIT * 1000) {
		tst_res(TPASS, "%s, unblocked within %ds",
			 test_cases[i].desc, MIN_TIME_LIMIT);
	} else {
		tst_res(TFAIL,
			"%s, blocked too long %llims, expected within %ds",
			test_cases[i].desc, elapsed_ms, MIN_TIME_LIMIT);
	}
}

void cleanup(void)
{
	if (sigprocmask(SIG_SETMASK, &oldset, NULL) < 0)
		tst_res(TWARN | TERRNO, "sigprocmask restore oldset failed");
	if (fd > 0)
		close(fd);
	FILE_PRINTF(PATH_LS_BRK_T, "%d", ls_brk_t);
}

void main(){
	setup();
	cleanup();
}
