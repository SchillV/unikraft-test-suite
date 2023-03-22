#include "incl.h"

int *fildes;

int min;

char pfilname[40];

void setup(void)
{
	fildes = malloc(min + 10) * sizeofint));
	memset(fildes, -1, (min + 10) * sizeof(int));
	sprintf(pfilname, "./dup205.%d\n", getpid());
}

void cleanup(void)
{
	if (fildes != NULL)
		free(fildes);
}

void run(void)
{
	int ifile = -1, rc = 0;
	fildes[0] = creat(pfilname, 0666);
	fildes[fildes[0]] = fildes[0];
	for (ifile = fildes[0] + 1; ifile < min + 10; ifile++) {
dup2(fildes[ifile - 1], ifile);
		fildes[ifile] = TST_RET;
		if (fildes[ifile] == -1)
			break;
		if (fildes[ifile] != ifile)
			tst_brk(TFAIL,
				"got wrong descriptor number back (%d != %d)",
				fildes[ifile], ifile);
	}
	if (ifile < min) {
		tst_res(TFAIL, "Not enough files duped");
		rc++;
	} else if (ifile > min) {
		tst_res(TFAIL, "Too many files duped");
		rc++;
	}
	if (TST_ERR != EBADF && TST_ERR != EMFILE && TST_ERR != EINVAL) {
		tst_res(TFAIL, "bad errno on dup2 failure");
		rc++;
	}
	if (rc)
		tst_res(TFAIL, "Test failed");
	else
		tst_res(TPASS, "Test passed");
	unlink(pfilname);
	for (ifile = fildes[0]; ifile < min + 10; ifile++) {
		if (fildes[ifile] > 0)
			close(fildes[ifile]);
	}
}

void main(){
	setup();
	cleanup();
}
