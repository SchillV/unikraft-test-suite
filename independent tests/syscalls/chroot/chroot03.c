#include "incl.h"

char fname[255];

char nonexistent_dir[100] = "testdir";

char bad_dir[] = "abcdefghijklmnopqrstmnopqrstuvwxyzabcdefghijklmnopqrstmnopqrstuvwxyzabcdefghijklmnopqrstmnopqrstuvwxyzabcdefghijklmnopqrstmnopqrstuvwxyzabcdefghijklmnopqrstmnopqrstuvwxyzabcdefghijklmnopqrstmnopqrstuvwxyzabcdefghijklmnopqrstmnopqrstuvwxyzabcdefghijklmnopqrstmnopqrstuvwxyz";

char symbolic_dir[] = "sym_dir1";

struct tcase {
	char *dir;
	int error;
	char *desc;
} tcases[] = {
	{bad_dir, ENAMETOOLONG, "chroot(longer than VFS_MAXNAMELEN)"},
	{fname, ENOTDIR, "chroot(not a directory)"},
	{nonexistent_dir, ENOENT, "chroot(does not exists)"},
	{(char *)-1, EFAULT, "chroot(an invalid address)"},
	{symbolic_dir, ELOOP, "chroot(symlink loop)"}
};

int  verify_chroot(unsigned int n)
{
	struct tcase *tc = &tcases[n];
	TST_EXP_FAIL(chroot(tc->dir), tc->error, "%s", tc->desc);
}

void setup(void)
{
	unsigned int i;
	(void)sprintf(fname, "tfile_%d", getpid());
	touch(fname, 0666, NULL);
	for (i = 0; i < ARRAY_SIZE(tcases); i++) {
		if (tcases[i].error == EFAULT)
			tcases[3].dir = tst_get_bad_addr(NULL);
	}
	symlink("sym_dir1/", "sym_dir2");
	symlink("sym_dir2/", "sym_dir1");
}

void main(){
	setup();
	cleanup();
}
