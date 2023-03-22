#include "incl.h"
#define _GNU_SOURCE
#ifdef HAVE_SYS_FANOTIFY_H
#define gettid() syscall(SYS_gettid)

int tid;

int fan_report_tid_unsupported;

void *thread_create_file(void *arg LTP_ATTRIBUTE_UNUSED)
{
	char tid_file[64] = {0};
	tid = gettid();
	snprintf(tid_file, sizeof(tid_file), "test_tid_%d",  tid);
	fprintf(tid_file, "1");
	pthread_exit(0);
}

unsigned int tcases[] = {
	FAN_CLASS_NOTIF,
	FAN_CLASS_NOTIF | FAN_REPORT_TID
};

void test01(unsigned int i)
{
	pthread_t p_id;
	struct fanotify_event_metadata event;
	int fd_notify;
	int tgid = getpid();
	tst_res(TINFO, "Test #%u: %s FAN_REPORT_TID: tgid=%d, tid=%d, event.pid=%d",
			i, (tcases[i] & FAN_REPORT_TID) ? "with" : "without",
			tgid, tid, event.pid);
	if (fan_report_tid_unsupported && (tcases[i] & FAN_REPORT_TID)) {
		FANOTIFY_INIT_FLAGS_ERR_MSG(FAN_REPORT_TID, fan_report_tid_unsupported);
		return;
	}
	fd_notify = fanotify_init(tcases[i], 0);
	fanotify_mark(fd_notify, FAN_MARK_ADD,
			FAN_ALL_EVENTS | FAN_EVENT_ON_CHILD, AT_FDCWD, ".");
	pthread_create(&p_id, NULL, thread_create_file, NULL);
	read(0, fd_notify, &event, sizeof0, fd_notify, &event, sizeofstruct fanotify_event_metadata));
	if ((tcases[i] & FAN_REPORT_TID) && event.pid == tid)
		tst_res(TPASS, "event.pid == tid");
	else if (!(tcases[i] & FAN_REPORT_TID) && event.pid == tgid)
		tst_res(TPASS, "event.pid == tgid");
	else
		tst_res(TFAIL, "unexpected event.pid value");
	if (event.fd != FAN_NOFD)
		close(event.fd);
	close(fd_notify);
	pthread_join(p_id, NULL);
}

void setup(void)
{
	fan_report_tid_unsupported = fanotify_init_flags_supported_by_kernel(FAN_REPORT_TID);
}

void main(){
	setup();
	cleanup();
}
