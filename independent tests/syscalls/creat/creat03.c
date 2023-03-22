#include "incl.h"

char pfilname[40];

int fd;

int  verify_creat(
{
	struct stat statbuf;
	fd = creat(pfilname, 444);
	if (fd == -1) {
		tst_res(TFAIL | TERRNO, "creat(%s) failed", pfilname);
		return;
	}
	fstat(fd, &statbuf);
	tst_res(TINFO, "Created file has mode = 0%o", statbuf.st_mode);
	if ((statbuf.st_mode & S_ISVTX) != 0)
		tst_res(TFAIL, "save text bit not cleared");
	else
		tst_res(TPASS, "save text bit cleared");
	close(fd);
}

void setup(void)
{
	sprintf(pfilname, "./creat03.%d", getpid());
}

void cleanup(void)
{
	if (fd > 0)
		close(fd);
}

void main(){
	setup();
	cleanup();
}
