#include "incl.h"

char testfile[40];

int ofd = -1, nfd = -1;

int duprdo, dupwro, duprdwr;

struct tcase {
	int *nfd;
	mode_t mode;
	int flag;
} tcases[] = {
	{&duprdo, 0444, 0},
	{&dupwro, 0222, 0},
	{&duprdwr, 0666, 0},
	{&duprdo, 0444, 1},
	{&dupwro, 0222, 1},
	{&duprdwr, 0666, 1},
};

void setup(void)
{
	int nextfd;
	umask(0);
	sprintf(testfile, "dup202.%d", getpid());
	nextfd = creat(testfile, 0777);
	duprdo = dup(nextfd);
	dupwro = dup(nextfd);
	duprdwr = dup(nextfd);
	close(duprdwr);
	close(dupwro);
	close(duprdo);
	close(nextfd);
	unlink(testfile);
}

void cleanup(void)
{
	close(ofd);
	close(nfd);
}

void run(unsigned int i)
{
	struct stat oldbuf, newbuf;
	struct tcase *tc = tcases + i;
	if (tc->flag)
		ofd = creat(testfile, 0777);
	else
		ofd = creat(testfile, tc->mode);
	nfd = *tc->nfd;
dup2(ofd, nfd);
	if (TST_RET == -1) {
		tst_res(TFAIL | TTERRNO, "call failed unexpectedly");
		goto free;
	}
	if (tc->flag) {
		chmod(testfile, tc->mode);
		tst_res(TINFO, "original mode 0777, new mode 0%o after chmod", tc->mode);
	}
	fstat(ofd, &oldbuf);
	fstat(nfd, &newbuf);
	if (oldbuf.st_mode != newbuf.st_mode)
		tst_res(TFAIL, "original(%o) and duped(%o) are not same mode",
			oldbuf.st_mode, newbuf.st_mode);
	else
		tst_res(TPASS, "original(%o) and duped(%o) are the same mode",
			oldbuf.st_mode, newbuf.st_mode);
	close(nfd);
free:
	close(ofd);
	unlink(testfile);
}

void main(){
	setup();
	cleanup();
}
