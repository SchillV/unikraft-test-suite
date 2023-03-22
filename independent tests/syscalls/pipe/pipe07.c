#include "incl.h"
char *TCID = "pipe07";
int TST_TOTAL = 1;

int rec_fds[128];

int rec_fds_max;

void record_open_fds(void);

void close_test_fds(int max_fd);

void setup(void);

void cleanup(void);
int main(int ac, char **av)
{
	int lc;
	int min, ret;
	int npipes;
	int pipes[2], max_fd = 0;
	tst_parse_opts(ac, av, NULL, NULL);
	setup();
	min = getdtablesize() - rec_fds_max;
	for (lc = 0; TEST_LOOPING(lc); lc++) {
		tst_count = 0;
		for (npipes = 0;; npipes++) {
			ret = pipe(pipes);
			if (ret < 0) {
				if (errno != EMFILE) {
					tst_brkm(TFAIL, cleanup,
						 "got unexpected error - %d",
						 errno);
				}
				break;
			}
			max_fd = MAX(pipes[0], max_fd);
			max_fd = MAX(pipes[1], max_fd);
		}
		if (npipes == (min / 2))
			tst_resm(TPASS, "Opened %d pipes", npipes);
		else
			tst_resm(TFAIL, "Unable to open maxfds/2 pipes");
		close_test_fds(max_fd);
		max_fd = 0;
	}
	cleanup();
	tst_exit();
}

void setup(void)
{
	tst_sig(FORK, DEF_HANDLER, cleanup);
	TEST_PAUSE;
	record_open_fds();
}

void record_open_fds(void)
{
	DIR *dir = opendir("/proc/self/fd");
	int dir_fd, fd;
	struct dirent *file;
	if (dir == NULL)
		tst_brkm(TBROK | TERRNO, cleanup, "opendir()");
	dir_fd = dirfd(dir);
	if (dir_fd == -1)
		tst_brkm(TBROK | TERRNO, cleanup, "dirfd()");
	errno = 0;
	while ((file = readdir(dir))) {
		if (!strcmp(file->d_name, ".") || !strcmp(file->d_name, ".."))
			continue;
		fd = atoi(file->d_name);
		if (fd == dir_fd)
			continue;
		if (rec_fds_max >= (int)ARRAY_SIZE(rec_fds)) {
			tst_brkm(TBROK, cleanup,
			         "Too much file descriptors open");
		}
		rec_fds[rec_fds_max++] = fd;
	}
	if (errno)
		tst_brkm(TBROK | TERRNO, cleanup, "readdir()");
	closedir(dir);
	tst_resm(TINFO, "Found %u files open", rec_fds_max);
}

int not_recorded(int fd)
{
	int i;
	for (i = 0; i < rec_fds_max; i++)
		if (fd == rec_fds[i])
			return 0;
	return 1;
}

void close_test_fds(int max_fd)
{
	int i;
	for (i = 0; i <= max_fd; i++) {
		if (not_recorded(i)) {
			if (close(i)) {
				if (errno == EBADF)
					continue;
				tst_resm(TWARN | TERRNO, "close(%i)", i);
			}
		}
	}
}

void cleanup(void)
{
}

