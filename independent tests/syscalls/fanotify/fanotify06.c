#include "incl.h"
#define _GNU_SOURCE
#ifdef HAVE_SYS_FANOTIFY_H
#define EVENT_MAX 1024
#define EVENT_SIZE  (sizeof(struct fanotify_event_metadata))
#define EVENT_BUF_LEN        (EVENT_MAX * EVENT_SIZE)

unsigned int fanotify_prio[] = {
	FAN_CLASS_PRE_CONTENT,
	FAN_CLASS_CONTENT,
	FAN_CLASS_NOTIF
};
#define FANOTIFY_PRIORITIES ARRAY_SIZE(fanotify_prio)
#define GROUPS_PER_PRIO 3
#define BUF_SIZE 256

char fname[BUF_SIZE];

int fd_notify[FANOTIFY_PRIORITIES][GROUPS_PER_PRIO];

char event_buf[EVENT_BUF_LEN];

const char mntpoint[] = OVL_BASE_MNTPOINT;

int ovl_mounted;

struct tcase {
	const char *tname;
	const char *mnt;
	int use_overlay;
} tcases[] = {
	{ "Fanotify merge mount mark", mntpoint, 0 },
	{ "Fanotify merge overlayfs mount mark", OVL_MNT, 1 },
};

void create_fanotify_groups(void)
{
	unsigned int p, i;
	for (p = 0; p < FANOTIFY_PRIORITIES; p++) {
		for (i = 0; i < GROUPS_PER_PRIO; i++) {
			fd_notify[p][i] = fanotify_init(fanotify_prio[p] |
							     FAN_NONBLOCK,
							     O_RDONLY);
			fanotify_mark(fd_notify[p][i],
					    FAN_MARK_ADD | FAN_MARK_MOUNT,
					    FAN_MODIFY,
					    AT_FDCWD, fname);
			if (p == 0)
				continue;
			fanotify_mark(fd_notify[p][i],
					    FAN_MARK_ADD |
					    FAN_MARK_IGNORED_MASK |
					    FAN_MARK_IGNORED_SURV_MODIFY,
					    FAN_MODIFY, AT_FDCWD, fname);
		}
	}
}

void cleanup_fanotify_groups(void)
{
	unsigned int i, p;
	for (p = 0; p < FANOTIFY_PRIORITIES; p++) {
		for (i = 0; i < GROUPS_PER_PRIO; i++) {
			if (fd_notify[p][i] > 0)
				close(fd_notify[p][i]);
		}
	}
}

int  verify_event(int group, struct fanotify_event_metadata *event)
{
	if (event->mask != FAN_MODIFY) {
		tst_res(TFAIL, "group %d got event: mask %llx (expected %llx) "
			"pid=%u fd=%d", group, (unsigned long long)event->mask,
			(unsigned long long)FAN_MODIFY,
			(unsigned int)event->pid, event->fd);
	} else if (event->pid != getpid()) {
		tst_res(TFAIL, "group %d got event: mask %llx pid=%u "
			"(expected %u) fd=%d", group,
			(unsigned long long)event->mask, (unsigned int)event->pid,
			(unsigned int)getpid(), event->fd);
	} else {
		tst_res(TPASS, "group %d got event: mask %llx pid=%u fd=%d",
			group, (unsigned long long)event->mask,
			(unsigned int)event->pid, event->fd);
	}
}

void close_events_fd(struct fanotify_event_metadata *event, int buflen)
{
	while (buflen >= (int)FAN_EVENT_METADATA_LEN) {
		if (event->fd != FAN_NOFD)
			close(event->fd);
		buflen -= (int)FAN_EVENT_METADATA_LEN;
		event++;
	}
}

void test_fanotify(unsigned int n)
{
	int ret;
	unsigned int p, i;
	struct fanotify_event_metadata *event;
	struct tcase *tc = &tcases[n];
	tst_res(TINFO, "Test #%d: %s", n, tc->tname);
	if (tc->use_overlay && !ovl_mounted) {
		tst_res(TCONF, "overlayfs is not configured in this kernel");
		return;
	}
	sprintf(fname, "%s/tfile_%d", tc->mnt, getpid());
	touch(fname, 0644, NULL);
	create_fanotify_groups();
	 * generate sequence of events
	 */
	fprintf(fname, "1");
	for (i = 0; i < GROUPS_PER_PRIO; i++) {
		ret = read(fd_notify[0][i], event_buf, EVENT_BUF_LEN);
		if (ret < 0) {
			if (errno == EAGAIN) {
				tst_res(TFAIL, "group %d did not get "
					"event", i);
			}
			tst_brk(TBROK | TERRNO,
				"reading fanotify events failed");
		}
		if (ret < (int)FAN_EVENT_METADATA_LEN) {
			tst_brk(TBROK,
				"short read when reading fanotify "
				"events (%d < %d)", ret,
				(int)EVENT_BUF_LEN);
		}
		event = (struct fanotify_event_metadata *)event_buf;
		if (ret > (int)event->event_len) {
			tst_res(TFAIL, "group %d got more than one "
				"event (%d > %d)", i, ret,
				event->event_len);
		} else {
int 			verify_event(i, event);
		}
		close_events_fd(event, ret);
	}
	for (p = 1; p < FANOTIFY_PRIORITIES; p++) {
		for (i = 0; i < GROUPS_PER_PRIO; i++) {
			ret = read(fd_notify[p][i], event_buf, EVENT_BUF_LEN);
			if (ret > 0) {
				tst_res(TFAIL, "group %d got event",
					p*GROUPS_PER_PRIO + i);
				close_events_fd((void *)event_buf, ret);
			} else if (ret == 0) {
				tst_brk(TBROK, "zero length "
					"read from fanotify fd");
			} else if (errno != EAGAIN) {
				tst_brk(TBROK | TERRNO,
					"reading fanotify events failed");
			} else {
				tst_res(TPASS, "group %d got no event",
					p*GROUPS_PER_PRIO + i);
			}
		}
	}
	cleanup_fanotify_groups();
}

void setup(void)
{
	ovl_mounted = TST_MOUNT_OVERLAY();
}

void cleanup(void)
{
	cleanup_fanotify_groups();
	if (ovl_mounted)
		umount(OVL_MNT);
}

void main(){
	setup();
	cleanup();
}
