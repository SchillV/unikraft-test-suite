#include "incl.h"
#define _GNU_SOURCE
#define PIDTRIES	3

char *last_pid_file;

int pidfd, new_pidfd;

int old_inode, new_inode;

int get_inode_number(int fd)
{
	struct stat file_stat;
	fstat(fd, &file_stat);
	return file_stat.st_ino;
}

int  verify_pidfd_send_signal(
{
	pid_t pid, new_pid;
	char pid_filename[32];
	char pid_str[16];
	int i, fail;
	fail = 1;
	for (i = 0; i < PIDTRIES; i++) {
		pid = fork();
		if (pid == 0) {
			TST_CHECKPOINT_WAIT(0);
			return;
		}
		sprintf(pid_filename, "/proc/%d", pid);
		pidfd = open(pid_filename, O_DIRECTORY | O_CLOEXEC);
		old_inode = get_inode_number(pidfd);
		TST_CHECKPOINT_WAKE(0);
		tst_reap_children();
		sprintf(pid_str, "%d", pid - 1);
		fprintf(last_pid_file, "%s", pid_str);
		new_pid = fork();
		if (new_pid == 0) {
			TST_CHECKPOINT_WAIT(0);
			return;
		}
		if (new_pid == pid) {
			new_pidfd = open(pid_filename,
					O_DIRECTORY | O_CLOEXEC);
			new_inode = get_inode_number(new_pidfd);
			close(new_pidfd);
			fail = 0;
			break;
		}
		if (i < PIDTRIES) {
			tst_res(TINFO,
				"Failed to set correct PID, trying again...");
		}
		close(pidfd);
		TST_CHECKPOINT_WAKE(0);
		tst_reap_children();
	}
	if (fail) {
		tst_brk(TBROK,
			"Could not set new child to same PID as the old one!");
	}
	if (old_inode == new_inode) {
		tst_res(TWARN,
			"File descriptor of new process points to the inode of the old process!");
	}
pidfd_send_signal(pidfd, SIGUSR1, NULL, 0);
	if (TST_RET == -1 && TST_ERR == ESRCH) {
		tst_res(TPASS,
			"Did not send signal to wrong process with same PID!");
	} else {
		tst_res(TFAIL | TTERRNO,
			"pidf_send_signal() ended unexpectedly - return value: %ld, error",
			TST_RET);
	}
	TST_CHECKPOINT_WAKE(0);
	tst_reap_children();
	close(pidfd);
}

void setup(void)
{
	pidfd_send_signal_supported();
	last_pid_file = "/proc/sys/kernel/ns_last_pid";
	if (access(last_pid_file, F_OK) == -1) {
		tst_brk(TCONF, "%s does not exist, cannot set PIDs",
			last_pid_file);
	}
}

void cleanup(void)
{
	tst_reap_children();
	if (new_pidfd > 0)
		close(new_pidfd);
	if (pidfd > 0)
		close(pidfd);
}

void main(){
	setup();
	cleanup();
}
