#include "incl.h"
#ifndef _GNU_SOURCE
#define _GNU_SOURCE
#endif

char nobody_uid[] = "nobody";

struct passwd *ltpuser;

char long_fname[] = "abcdefghijklmnopqrstuvwxyzabcdefghijklmnopqrstuvwxyzabcdefghijklmnopqrstmnopqrstuvwxyzabcdefghijklmnopqrstmnopqrstuvwxyzabcdefghijklmnopqrstmnopqrstuvwxyzabcdefghijklmnopqrstmnopqrstuvwxyzabcdefghijklmnopqrstmnopqrstuvwxyzabcdefghijklmnopqrstmnopqrstuvwxyz";

char no_dir[] = "testdir";

char test_name3[1024];

char test_name5[1024];

char test_name6[1024];

struct tcase {
	char *tname;
	int error;
} tcases[] = {
	{long_fname, ENAMETOOLONG},
	{no_dir, ENOENT},
	{test_name3, ENOTDIR},
	{NULL, EFAULT},
	{test_name5, EACCES},
	{test_name6, ENOEXEC}
};

void setup(void)
{
	char *cwdname = NULL;
	unsigned i;
	int fd;
	umask(0);
	ltpuser = getpwnam(nobody_uid);
	setgid(ltpuser->pw_gid);
	cwdname = getcwd(cwdname, 0);
	sprintf(test_name5, "%s/fake", cwdname);
	fd = creat(test_name5, 0444);
	close(fd);
	sprintf(test_name3, "%s/fake", test_name5);
	sprintf(test_name6, "%s/execve03", cwdname);
	fd = creat(test_name6, 0755);
	close(fd);
	for (i = 0; i < 6; i++) {
		if (!tcases[i].tname)
			tcases[i].tname = NULL;
	}
}

int  verify_execve(unsigned int i)
{
	struct tcase *tc = &tcases[i];
	char *argv[2] = {tc->tname, NULL};
	int ret = execve(tc->tname, argv, NULL);
	if (ret != -1) {
		printf("call succeeded unexpectedly\n");
		return 0;
	}
	if (errno == tc->error) {
		printf("execve failed as expected\n");
		return 1;
	}
	printf("execve failed unexpectedly; expected %d, got %d\n", tc->error, errno);
	return 0;
}

void main(){
	int ok=1;
	setup();
	for(int i = 0; i<6; i++)
		if(verify_execve(i) == 0)
			ok = 0;
	if(ok)
		printf("test succeeded\n");
}
