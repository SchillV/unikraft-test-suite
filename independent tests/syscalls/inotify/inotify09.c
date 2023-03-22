#include "incl.h"
#define FNAME "stress_fname"
#if defined(HAVE_SYS_INOTIFY_H)

struct tst_fzsync_pair fzsync_pair;

int fd;

void *write_seek(void *unused)
{
	char buf[64];
	while (tst_fzsync_run_b(&fzsync_pair)) {
		tst_fzsync_start_race_b(&fzsync_pair);
		write(
		lseek(fd, 0, SEEK_SET);
		tst_fzsync_end_race_b(&fzsync_pair);
	}
	return unused;
}

void setup(void)
{
	fd = open(FNAME, O_CREAT | O_RDWR, 0600);
	tst_fzsync_pair_init(&fzsync_pair);
}

void cleanup(void)
{
	if (fd > 0)
		close(fd);
	tst_fzsync_pair_cleanup(&fzsync_pair);
}

int  verify_inotify(
{
	int inotify_fd;
	int wd;
	inotify_fd = myinotify_init1(0);
	tst_fzsync_pair_reset(&fzsync_pair, write_seek);
	while (tst_fzsync_run_a(&fzsync_pair)) {
		wd = myinotify_add_watch(inotify_fd, FNAME, IN_MODIFY);
		tst_fzsync_start_race_a(&fzsync_pair);
		wd = myinotify_rm_watch(inotify_fd, wd);
		tst_fzsync_end_race_a(&fzsync_pair);
		if (wd < 0)
			tst_brk(TBROK | TERRNO, "inotify_rm_watch() failed.");
	}
	close(inotify_fd);
	tst_res(TPASS, "kernel survived inotify beating");
}

void main(){
	setup();
	cleanup();
}
