#include "incl.h"
#define BASEMODNAME	"dummy"
#define LONGMODNAMECHAR	'm'
#define MODULE_NAME_LEN	(64 - sizeof(unsigned long))

struct passwd *ltpuser;

char longmodname[MODULE_NAME_LEN];

char modname[20];

struct test_case_t {
	char *modname;
	int experrno;
	char *desc;
	int nobody_user;
} tdat[] = {
	{ modname, ENOENT, "nonexistent module", 0},
	{ "", ENOENT, "null terminated module name", 0},
	{ NULL, EFAULT, "module name outside program's accessible address space", 0},
	{ longmodname, ENOENT, "long module name", 0},
	{ modname, EPERM, "non-superuser", 1},
};

void do_delete_module(unsigned int n)
{
	struct test_case_t *tc = &tdat[n];
	if (!tc->modname)
		tc->modname = tst_get_bad_addr(NULL);
	if (tc->nobody_user)
		seteuid(ltpuser->pw_uid);
	tst_res(TINFO, "test %s", tc->desc);
tst_syscall(__NR_delete_module, tc->modname, 0);
	if (TST_RET != -1) {
		tst_res(TFAIL, "delete_module() succeeded unexpectedly");
	} else if (TST_ERR == tc->experrno) {
		tst_res(TPASS | TTERRNO, "delete_module() failed as expected");
	} else {
		tst_res(TFAIL | TTERRNO, "delete_module() failed unexpectedly;"
			" expected: %s", tst_strerrno(tc->experrno));
	}
	if (tc->nobody_user)
		seteuid(0);
}

void setup(void)
{
	ltpuser = getpwnam("nobody");
	memset(longmodname, LONGMODNAMECHAR, MODULE_NAME_LEN - 1);
	sprintf(modname, "%s_%d", BASEMODNAME, getpid());
}

void main(){
	setup();
	cleanup();
}
