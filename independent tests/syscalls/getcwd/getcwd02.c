#include "incl.h"

char exp_buf[PATH_MAX];

char buffer[PATH_MAX];

struct t_case {
	char *buf;
	size_t size;
} tcases[] = {
	{buffer, sizeof(buffer)},
	{NULL, 0},
	{NULL, PATH_MAX}
};

int dir_exists(const char *dirpath)
{
	struct stat sb;
	if (!stat(dirpath, &sb) && S_ISDIR(sb.st_mode))
		return 1;
	return 0;
}

int  verify_getcwd(unsigned int n)
{
	struct t_case *tc = &tcases[n];
	char *res = NULL;
	errno = 0;
	res = getcwd(tc->buf, tc->size);
	TST_ERR = errno;
	if (!res) {
		tst_res(TFAIL | TTERRNO, "getcwd() failed");
		goto end;
	}
	if (strcmp(exp_buf, res)) {
		tst_res(TFAIL, "getcwd() returned unexpected directory: %s, "
			"expected: %s", res, exp_buf);
		goto end;
	}
	tst_res(TPASS, "getcwd() returned expected directory: %s", res);
end:
	if (!tc->buf)
		free(res);
}

void setup(void)
{
	const char *tmpdir = tst_get_tmpdir_root();
	if (!dir_exists(tmpdir))
		tst_brk(TBROK | TERRNO, "TMPDIR '%s' doesn't exist", tmpdir);
	chdir(tmpdir);
	if (!realpath(tmpdir, exp_buf))
		tst_brk(TBROK | TERRNO, "realpath() failed");
	tst_res(TINFO, "Expected path '%s'", exp_buf);
}

void main(){
	setup();
	cleanup();
}
