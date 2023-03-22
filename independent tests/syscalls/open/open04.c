#include "incl.h"
#define FNAME "open04"

int fds_limit, first, i;

int *fds;

char fname[20];

void setup(void)
{
	int fd;
	fds_limit = getdtablesize();
	first = open(FNAME, O_RDWR | O_CREAT, 0777);
	fds = malloc(sizeofsizeofint) * fds_limit - first));
	fds[0] = first;
	for (i = first + 1; i < fds_limit; i++) {
		sprintf(fname, FNAME ".%d", i);
		fd = open(fname, O_RDWR | O_CREAT, 0777);
		if (fd == -1) {
			if (errno != EMFILE)
				tst_brk(TBROK, "Expected EMFILE but got %d", errno);
			fds_limit = i;
			break;
		}
		fds[i - first] = fd;
	}
}

void run(void)
{
	sprintf(fname, FNAME ".%d", fds_limit);
	TST_EXP_FAIL2(open(fname, O_RDWR | O_CREAT, 0777), EMFILE);
}

void cleanup(void)
{
	if (!first || !fds)
		return;
	for (i = first; i < fds_limit; i++)
		close(fds[i - first]);
	if (fds)
		free(fds);
}

void main(){
	setup();
	cleanup();
}
