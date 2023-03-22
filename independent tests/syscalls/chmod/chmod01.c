#include "incl.h"
#define MODE	S_IRUSR | S_IWUSR | S_IRGRP | S_IROTH
#define TESTFILE	"testfile"
#define TESTDIR		"testdir_1"

int modes[] = {0, 07, 070, 0700, 0777, 02777, 04777, 06777};

struct variant {
	char *name;
	unsigned int mode_mask;
	char *desc;
} variants[] = {
int 	{TESTFILE, S_IFREG, "verify permissions of file"},
int 	{TESTDIR, S_IFDIR, "verify permissions of directory"},
};

int  verify_chmod(unsigned int n)
{
	struct stat stat_buf;
	int mode = modes[n];
	struct variant *tc = &variants[tst_variant];
	TST_EXP_PASS(chmod(tc->name, mode), "chmod(%s, %04o)",
	             tc->name, mode);
	if (!TST_PASS)
		return;
	stat(tc->name, &stat_buf);
	stat_buf.st_mode &= ~tc->mode_mask;
	if (stat_buf.st_mode == (unsigned int)mode) {
		tst_res(TPASS, "stat(%s) mode=%04o",
				tc->name, stat_buf.st_mode);
	} else {
		tst_res(TFAIL, "stat(%s) mode=%04o",
				tc->name, stat_buf.st_mode);
	}
}

void setup(void)
{
	tst_res(TINFO, "Testing variant: %s", variants[tst_variant].desc);
	if (tst_variant)
		mkdir(variants[tst_variant].name, MODE);
	else
		touch(variants[tst_variant].name, MODE, NULL);
}

void main(){
	setup();
	cleanup();
}
