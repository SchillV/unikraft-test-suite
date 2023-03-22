#include "incl.h"

char long_dir[] = "abcdefghijklmnopqrstmnopqrstuvwxyzabcdefghijklmnopqrstmnopqrstuvwxyzabcdefghijklmnopqrstmnopqrstuvwxyzabcdefghijklmnopqrstmnopqrstuvwxyzabcdefghijklmnopqrstmnopqrstuvwxyzabcdefghijklmnopqrstmnopqrstuvwxyzabcdefghijklmnopqrstmnopqrstuvwxyzabcdefghijklmnopqrstmnopqrstuvwxyz";

char noexist_dir[] = "noexistdir";

struct tcase {
	char *dir;
	int exp_errno;
} tcases[] = {
	{long_dir, ENAMETOOLONG},
	{noexist_dir, ENOENT},
};

int  verify_chdir(unsigned int i)
{
	TST_EXP_FAIL(chdir(tcases[i].dir), tcases[i].exp_errno, "chdir()");
}

void setup(void)
{
	tcases[2].dir = tst_get_bad_addr(NULL);
}

void main(){
	setup();
	cleanup();
}
