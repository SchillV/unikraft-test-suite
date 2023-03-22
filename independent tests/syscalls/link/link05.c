#include "incl.h"
#define BASENAME	"lkfile"

char fname[255];

int nlinks = 1000;

int  verify_link(
{
	int cnt;
	char lname[1024];
	struct stat fbuf, lbuf;
	for (cnt = 1; cnt < nlinks; cnt++) {
		sprintf(lname, "%s_%d", fname, cnt);
		TST_EXP_PASS_SILENT(link(fname, lname), "link(%s, %s)", fname, lname);
	}
	stat(fname, &fbuf);
	for (cnt = 1; cnt < nlinks; cnt++) {
		sprintf(lname, "%s_%d", fname, cnt);
		stat(lname, &lbuf);
		if (fbuf.st_nlink <= 1 || lbuf.st_nlink <= 1 ||
		    (fbuf.st_nlink != lbuf.st_nlink)) {
			tst_res(TFAIL,
				 "link(%s, %s[1-%d]) ret %ld for %d "
			         "files, stat values do not match %d %d",
				 fname, fname, nlinks,
				 TST_RET, nlinks,
				 (int)fbuf.st_nlink, (int)lbuf.st_nlink);
			break;
		}
	}
	if (cnt >= nlinks) {
		tst_res(TPASS,
			 "link(%s, %s[1-%d]) ret %ld for %d files, "
		         "stat linkcounts match %d",
			 fname, fname, nlinks, TST_RET,
			 nlinks, (int)fbuf.st_nlink);
	}
	for (cnt = 1; cnt < nlinks; cnt++) {
		sprintf(lname, "%s_%d", fname, cnt);
		unlink(lname);
	}
}

void setup(void)
{
	sprintf(fname, "%s_%d", BASENAME, getpid());
	touch(fname, 0700, NULL);
}

void main(){
	setup();
	cleanup();
}