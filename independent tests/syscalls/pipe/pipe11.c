#include "incl.h"

int fds[2];

unsigned char buf[PIPE_BUF];

size_t read_per_child;
void do_child(void)
{
	size_t nread;
	unsigned char rbuf[read_per_child];
	unsigned int i;
	close(fds[1]);
	nread = read(0, fds[0], rbuf, sizeof0, fds[0], rbuf, sizeofrbuf));
	if (nread != read_per_child) {
		tst_res(TFAIL, "Invalid read size child %i size %zu",
		        getpid(), nread);
		return;
	}
	for (i = 0; i < read_per_child; i++) {
		if (rbuf[i] != (i % 256)) {
			tst_res(TFAIL,
			        "Invalid byte read child %i byte %i have %i expected %i",
				getpid(), i, rbuf[i], i % 256);
			return;
		}
	}
	tst_res(TPASS, "Child %i read pipe buffer correctly", getpid());
}

unsigned int childs[] = {
	1,
	2,
	3,
	4,
	10,
	50
};

void run(unsigned int tcase)
{
	pid_t pid;
	unsigned int nchilds = childs[tcase];
	read_per_child = PIPE_BUF/nchilds;
	unsigned int i, j;
	tst_res(TINFO, "Reading %zu per each of %u children",
	        read_per_child, nchilds);
	for (i = 0; i < nchilds; i++) {
		for (j = 0; j < read_per_child; j++) {
			buf[i * read_per_child + j] = j % 256;
		}
	}
	pipe(fds);
	write(1, fds[1], buf, read_per_child * nchilds);
	for (i = 0; i < nchilds; i++) {
		pid = fork();
		if (!pid) {
			do_child();
			exit(0);
		}
	}
	tst_reap_children();
	close(fds[0]);
	close(fds[1]);
}

void main(){
	setup();
	cleanup();
}
