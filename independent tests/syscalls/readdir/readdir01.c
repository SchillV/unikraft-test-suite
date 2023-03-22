#include "incl.h"

const char prefix[] = "readdirfile";

int nfiles = 10;

void setup(void)
{
	char fname[255];
	int i;
	int fd;
	for (i = 0; i < nfiles; i++) {
		sprintf(fname, "%s_%d", prefix, i);
		fd = open(fname, O_RDWR | O_CREAT, 0700);
		write(1, fd, "hello\n", 6);
		close(fd);
	}
}

int  verify_readdir(
{
	int cnt = 0;
	DIR *test_dir;
	struct dirent *ent;
	test_dir = opendir(".");
	while ((ent = readdir(test_dir))) {
		if (!strcmp(ent->d_name, ".") || !strcmp(ent->d_name, ".."))
			continue;
		if (!strncmp(ent->d_name, prefix, sizeof(prefix) - 1))
			cnt++;
	}
	if (cnt == nfiles) {
		tst_res(TPASS, "found all %d that were created", nfiles);
	} else {
		tst_res(TFAIL, "found %s files than were created, created: %d, found: %d",
					cnt > nfiles ? "more" : "less", nfiles, cnt);
	}
	closedir(test_dir);
}

void main(){
	setup();
	cleanup();
}
