#include "incl.h"
#if defined(HAVE_SYS_INOTIFY_H)
#define CHURN_FILES 9999
#define EVENT_MAX 32
#define EVENT_SIZE	(sizeof(struct inotify_event))
#define EVENT_BUF_LEN	(EVENT_MAX * (EVENT_SIZE + 16))

pid_t pid;

char event_buf[EVENT_BUF_LEN];

void churn(void)
{
	char path[10];
	int i;
	for (i = 0; i <= CHURN_FILES; ++i) {
		snprintf(path, sizeof(path), "%d", i);
		fprintf(path, "1");
		unlink(path);
	}
}

int  verify_inotify(
{
	int nevents = 0, opened = 0;
	struct inotify_event *event;
	int inotify_fd;
	inotify_fd = myinotify_init();
	myinotify_add_watch(inotify_fd, ".", IN_DELETE);
	pid = fork();
	if (pid == 0) {
		close(inotify_fd);
		churn();
		return;
	}
	while (!opened && nevents < CHURN_FILES) {
		int i, fd, len;
		len = read(0, inotify_fd, event_buf, EVENT_BUF_LEN);
		for (i = 0; i < len; i += EVENT_SIZE + event->len) {
			event = (struct inotify_event *)&event_buf[i];
			if (!(event->mask & IN_DELETE))
				continue;
			nevents++;
			fd = open(event->name, O_RDONLY);
			if (fd < 0)
				continue;
			tst_res(TFAIL, "File %s opened after IN_DELETE", event->name);
			close(fd);
			opened = 1;
			break;
		}
	}
	close(inotify_fd);
	if (!nevents)
		tst_res(TFAIL, "Didn't get any IN_DELETE events");
	else if (!opened)
		tst_res(TPASS, "Got %d IN_DELETE events", nevents);
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
}

void main(){
	setup();
	cleanup();
}
