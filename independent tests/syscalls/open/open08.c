#include "incl.h"
#define FLAGS_DESC(x) .flags = x, .desc = #x

char *existing_fname = "open08_testfile";

char *toolong_fname = "abcdefghijklmnopqrstuvwxyzabcdefghijklmnopqrstuvwxyzabcdefghijklmnopqrstmnopqrstuvwxyzabcdefghijklmnopqrstmnopqrstuvwxyzabcdefghijklmnopqrstmnopqrstuvwxyzabcdefghijklmnopqrstmnopqrstuvwxyzabcdefghijklmnopqrstmnopqrstuvwxyzabcdefghijklmnopqrstmnopqrstuvwxyz";

char *dir_fname = "/tmp";

char *user2_fname = "user2_0600";

char *unmapped_fname;
struct test_case_t;

struct test_case_t {
	char **fname;
	int flags;
	const char *desc;
	int error;
} tcases[] = {
	{&existing_fname, FLAGS_DESC(O_CREAT | O_EXCL), EEXIST},
	{&dir_fname, FLAGS_DESC(O_RDWR), EISDIR},
	{&existing_fname, FLAGS_DESC(O_DIRECTORY), ENOTDIR},
	{&toolong_fname, FLAGS_DESC(O_RDWR), ENAMETOOLONG},
	{&user2_fname, FLAGS_DESC(O_WRONLY), EACCES},
	{&unmapped_fname, FLAGS_DESC(O_CREAT), EFAULT},
};

int  verify_open(unsigned int i)
{
	TST_EXP_FAIL2(open(*tcases[i].fname, tcases[i].flags, 0644),
				tcases[i].error, "%s", tcases[i].desc);
}

void setup(void)
{
	int fildes;
	char nobody_uid[] = "nobody";
	struct passwd *ltpuser;
	umask(0);
	creat(user2_fname, 0600);
	ltpuser = getpwnam(nobody_uid);
	setgid(ltpuser->pw_gid);
	setuid(ltpuser->pw_uid);
	fildes = creat(existing_fname, 0600);
	close(fildes);
	unmapped_fname = tst_get_bad_addr(NULL);
}

void main(){
	setup();
	cleanup();
}
