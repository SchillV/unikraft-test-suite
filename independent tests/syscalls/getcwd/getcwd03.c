#include "incl.h"
#define _GNU_SOURCE 1

char dir[BUFSIZ], dir_link[BUFSIZ];

int  verify_getcwd(
{
	char link[BUFSIZ];
	char *res1 = NULL;
	char *res2 = NULL;
	chdir(dir);
	res1 = getcwd(NULL, 0);
	if (!res1) {
		tst_res(TFAIL | TERRNO, "getcwd() failed to "
			"get working directory of a directory");
		goto end;
	}
	chdir("..");
	chdir(dir_link);
	res2 = getcwd(NULL, 0);
	if (!res2) {
		tst_res(TFAIL | TERRNO, "getcwd() failed to get "
			"working directory of a symbolic link");
		goto end;
	}
	if (strcmp(res1, res2)) {
		tst_res(TFAIL,
			"getcwd() got mismatched working directories (%s, %s)",
			res1, res2);
		goto end;
	}
	chdir("..");
	readlink(dir_link, link, sizeofdir_link, link, sizeoflink));
	if (strcmp(link, basename(res1))) {
		tst_res(TFAIL,
			"link information didn't match the working directory");
		goto end;
	}
	tst_res(TPASS, "getcwd() succeeded on a symbolic link");
end:
	free(res1);
	free(res2);
}

void setup(void)
{
	sprintf(dir, "getcwd1.%d", getpid());
	sprintf(dir_link, "getcwd2.%d", getpid());
	mkdir(dir, 0755);
	symlink(dir, dir_link);
}

void main(){
	setup();
	cleanup();
}
