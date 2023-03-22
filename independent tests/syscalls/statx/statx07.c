#include "incl.h"
#define _GNU_SOURCE
#define MODE(X) (X & (~S_IFMT))
#define FLAG_NAME(x) .flag = x, .flag_name = #x
#define BUFF_SIZE PATH_MAX
#define DEFAULT_MODE 0644
#define CURRENT_MODE 0777
#define CLI_PATH "client"
#define SERV_PATH "server"
#define CLI_FORCE_SYNC "client/force_sync_file"
#define CLI_DONT_SYNC "client/dont_sync_file"
#define SERV_FORCE_SYNC "server/force_sync_file"
#define SERV_DONT_SYNC "server/dont_sync_file"

char *cwd;

char cmd[BUFF_SIZE];

int mounted;

int exported;

int get_mode(char *file_name, int flag_type, char *flag_name)
{
	struct statx buf;
statx(AT_FDCWD, file_name, flag_type, STATX_ALL, &buf);
	if (TST_RET == -1) {
		tst_brk(TFAIL | TST_ERR,
			"statx(AT_FDCWD, %s, %s, STATX_ALL, &buf)",
			file_name, flag_name);
	}
	tst_res(TINFO, "statx(AT_FDCWD, %s, %s, STATX_ALL, &buf) = %o",
		file_name, flag_name, buf.stx_mode);
	return buf.stx_mode;
}

const struct test_cases {
	int flag;
	char *flag_name;
	char *server_file;
	char *client_file;
	unsigned int mode;
} tcases[] = {
	{FLAG_NAME(AT_STATX_DONT_SYNC), SERV_DONT_SYNC, CLI_DONT_SYNC, DEFAULT_MODE},
	{FLAG_NAME(AT_STATX_FORCE_SYNC), SERV_FORCE_SYNC, CLI_FORCE_SYNC, CURRENT_MODE}
};

void test_statx(unsigned int i)
{
	const struct test_cases *tc = &tcases[i];
	unsigned int cur_mode;
	get_mode(tc->client_file, AT_STATX_FORCE_SYNC, "AT_STATX_FORCE_SYNC");
	chmod(tc->server_file, CURRENT_MODE);
	cur_mode = get_mode(tc->client_file, tc->flag, tc->flag_name);
	if (MODE(cur_mode) == tc->mode) {
		tst_res(TPASS,
			"statx() with %s for mode %o",
			tc->flag_name, tc->mode);
	} else {
		tst_res(TFAIL,
			"statx() with %s for mode %o %o",
			tc->flag_name, tc->mode, MODE(cur_mode));
	}
	chmod(tc->server_file, DEFAULT_MODE);
}

void setup(void)
{
	int ret;
	char server_path[BUFF_SIZE];
	cwd = tst_get_tmpdir();
	mkdir(SERV_PATH, DEFAULT_MODE);
	mkdir(CLI_PATH, DEFAULT_MODE);
	creat(SERV_FORCE_SYNC, DEFAULT_MODE);
	creat(SERV_DONT_SYNC, DEFAULT_MODE);
	snprintf(server_path, sizeof(server_path), ":%s/%s", cwd, SERV_PATH);
	snprintf(cmd, sizeof(cmd),
		 "exportfs -i -o no_root_squash,rw,sync,no_subtree_check,fsid=%d *%.1024s",
		 getpid(), server_path);
	exported = 1;
	ret = tst_system(cmd);
	if (ret)
		tst_brk(TBROK | TST_ERR, "failed to exportfs");
	if (mount(server_path, CLI_PATH, "nfs", 0, "addr=127.0.0.1")) {
		if (errno == EOPNOTSUPP || errno == ECONNREFUSED
			|| errno == ETIMEDOUT)
			tst_brk(TCONF | TERRNO, "nfs server not set up?");
		tst_brk(TBROK | TERRNO, "mount() nfs failed");
	}
	mounted = 1;
}

void cleanup(void)
{
	if (mounted)
		umount(CLI_PATH);
	if (!exported)
		return;
	snprintf(cmd, sizeof(cmd),
		 "exportfs -u *:%s/%s", cwd, SERV_PATH);
	if (tst_system(cmd) == -1)
		tst_res(TWARN | TST_ERR, "failed to clear exportfs");
}

void main(){
	setup();
	cleanup();
}
