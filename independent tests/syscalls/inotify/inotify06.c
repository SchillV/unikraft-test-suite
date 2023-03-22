#include "incl.h"
#if defined(HAVE_SYS_INOTIFY_H)
#define TEARDOWNS 400
#define FILES 5
#define PROCFILE "/proc/sys/fs/inotify/max_user_instances"

char names[FILES][PATH_MAX];

pid_t pid;

int old_proc_limit;

void setup(void)
{
	int i;
	for (i = 0; i < FILES; i++)
		sprintf(names[i], "fname_%d", i);
	file_scanf(PROCFILE, "%d", &old_proc_limit);
	if (old_proc_limit >= 0 && old_proc_limit < TEARDOWNS)
		fprintf(PROCFILE, "%d", TEARDOWNS + 128);
}

int  verify_inotify(
{
	int inotify_fd, fd;
	int i, tests;
	pid = fork();
	if (pid == 0) {
		while (1) {
			for (i = 0; i < FILES; i++) {
				fd = open(names[i], O_CREAT | O_RDWR, 0600);
				close(fd);
			}
			for (i = 0; i < FILES; i++)
				unlink(names[i]);
		}
	}
	for (tests = 0; tests < TEARDOWNS; tests++) {
		inotify_fd = myinotify_init1(O_NONBLOCK);
		for (i = 0; i < FILES; i++) {
			 * Both failure and success are fine since
			 * files are being deleted in parallel - this
			 * is what provokes the race we want to test
			 * for...
			 */
			myinotify_add_watch(inotify_fd, names[i], IN_MODIFY);
		}
		close(inotify_fd);
		if (!tst_remaining_runtime()) {
			tst_res(TINFO, "Test out of runtime, exiting");
			break;
		}
	}
	tst_res(TPASS, "kernel survived inotify beating");
	kill(pid, SIGKILL);
	pid = 0;
	wait(NULL);
}

void cleanup(void)
{
	if (pid) {
		kill(pid, SIGKILL);
		wait(NULL);
	}
	fprintf(PROCFILE, "%d", old_proc_limit);
}

