#include "incl.h"

struct passwd *pw;

struct test_case_t {
	char *name;
	char *desc;
	int exp_errno;
	int exp_user;
} tcases[] = {
	{"unwrite_dir/file", "unwritable directory", EACCES, 1},
	{"unsearch_dir/file", "unsearchable directory", EACCES, 1},
	{"regdir", "directory", EISDIR, 0},
	{"regdir", "directory", EISDIR, 1},
};

int  verify_unlink(struct test_case_t *tc)
{
	TST_EXP_FAIL(unlink(tc->name), tc->exp_errno, "%s", tc->desc);
}

void do_unlink(unsigned int n)
{
	struct test_case_t *cases = &tcases[n];
	pid_t pid;
	if (cases->exp_user) {
		pid = fork();
		if (!pid) {
			setuid(pw->pw_uid);
int 			verify_unlink(cases);
			exit(0);
		}
		waitpid(pid, NULL, 0);
	} else {
int 		verify_unlink(cases);
	}
}

void setup(void)
{
	mkdir("unwrite_dir", 0777);
	touch("unwrite_dir/file", 0777, NULL);
	chmod("unwrite_dir", 0555);
	mkdir("unsearch_dir", 0777);
	touch("unsearch_dir/file", 0777, NULL);
	chmod("unsearch_dir", 0666);
	mkdir("regdir", 0777);
	pw = getpwnam("nobody");
}

void main(){
	setup();
	cleanup();
}
