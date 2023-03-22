#include "incl.h"
#define TEMP_FILE	"testfile"

char nobody_uid[] = "nobody";

struct passwd *ltpuser;

int fd = -1;

void setup(void)
{
	ltpuser = getpwnam(nobody_uid);
	UID16_CHECK(ltpuser->pw_uid, "setresuid");
	fd = open(TEMP_FILE, O_CREAT | O_RDWR, 0644);
}

void run(void)
{
	pid_t pid;
	int status;
	TST_EXP_PASS_SILENT(SETRESUID(-1, ltpuser->pw_uid, -1));
	TST_EXP_FAIL2(open(TEMP_FILE, O_RDWR), EACCES);
	pid = fork();
	if (!pid) {
		TST_EXP_FAIL2(open(TEMP_FILE, O_RDWR), EACCES);
		return;
	}
	waitpid(pid, &status, 0);
	if (WIFEXITED(status) && WEXITSTATUS(status) != 0)
		tst_res(TFAIL, "child process exited with status: %d", status);
	TST_EXP_PASS_SILENT(SETRESUID(-1, 0, -1));
	TST_EXP_FD(open(TEMP_FILE, O_RDWR));
	close(TST_RET);
}

void cleanup(void)
{
	if (fd > 0)
		close(fd);
}

void main(){
	setup();
	cleanup();
}
