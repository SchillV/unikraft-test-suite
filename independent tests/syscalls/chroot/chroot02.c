#include "incl.h"
#define TMP_FILENAME	"chroot02_testfile"

char *path;

int  verify_chroot(
{
	struct stat buf;
	if (!fork()) {
		TST_EXP_PASS(chroot(path), "chroot(%s)", path);
		if (!TST_PASS)
			return;
		TST_EXP_PASS(stat("/" TMP_FILENAME, &buf), "stat(/%s)", TMP_FILENAME);
	}
}

void setup(void)
{
	path = tst_get_tmpdir();
	touch(TMP_FILENAME, 0666, NULL);
}

void cleanup(void)
{
	free(path);
}

void main(){
	setup();
	cleanup();
}
