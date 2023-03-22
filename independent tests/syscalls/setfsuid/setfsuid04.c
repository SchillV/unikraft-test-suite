#include "incl.h"
TCID_DEFINE(setfsuid04);
int TST_TOTAL = 1;

char nobody_uid[] = "nobody";

char testfile[] = "setfsuid04_testfile";

struct passwd *ltpuser;

int fd = -1;

void setup(void);

void cleanup(void);

void do_master_child(void);
int main(int ac, char **av)
{
	pid_t pid;
	tst_parse_opts(ac, av, NULL, NULL);
	setup();
	pid = FORK_OR_VFORK();
	if (pid < 0)
		tst_brkm(TBROK, cleanup, "Fork failed");
	if (pid == 0)
		do_master_child();
	tst_record_childstatus(cleanup, pid);
	cleanup();
	tst_exit();
}

void do_master_child(void)
{
	int pid;
	int status;
	if (SETFSUID(NULL, ltpuser->pw_uid) == -1) {
		perror("setfsuid failed");
		exit(TFAIL);
	}
	 *         with RDWR permissions.
	 */
open(testfile, O_RDWR);
	if (TEST_RETURN != -1) {
		close(TEST_RETURN);
		printf("open succeeded unexpectedly\n");
		exit(TFAIL);
	}
	if (TEST_ERRNO == EACCES) {
		printf("open failed with EACCESS as expected\n");
	} else {
		printf("open returned unexpected errno - %d\n", TEST_ERRNO);
		exit(TFAIL);
	}
	 *         with RDWR permissions.
	 */
	pid = FORK_OR_VFORK();
	if (pid < 0) {
		perror("Fork failed");
		exit(TFAIL);
	}
	if (pid == 0) {
open(testfile, O_RDWR);
		if (TEST_RETURN != -1) {
			close(TEST_RETURN);
			printf("open succeeded unexpectedly\n");
			exit(TFAIL);
		}
		if (TEST_ERRNO == EACCES) {
			printf("open failed with EACCESS as expected\n");
		} else {
			printf("open returned unexpected errno - %d\n",
			       TEST_ERRNO);
			exit(TFAIL);
		}
	} else {
		if (waitpid(pid, &status, 0) == -1) {
			perror("waitpid failed");
			exit(TFAIL);
		}
		if (!WIFEXITED(status) || WEXITSTATUS(status) != 0)
			exit(WEXITSTATUS(status));
	}
	 *         the file with RDWR permissions.
	 */
	tst_count++;
	if (SETFSUID(NULL, 0) == -1) {
		perror("setfsuid failed");
		exit(TFAIL);
	}
open(testfile, O_RDWR);
	if (TEST_RETURN == -1) {
		perror("open failed unexpectedly");
		exit(TFAIL);
	} else {
		printf("open call succeeded\n");
		close(TEST_RETURN);
	}
	exit(TPASS);
}

void setup(void)
{
	tst_require_root();
	ltpuser = getpwnam(nobody_uid);
	if (ltpuser == NULL)
		tst_brkm(TBROK, cleanup, "getpwnam failed for user id %s",
			nobody_uid);
	UID16_CHECK(ltpuser->pw_uid, setfsuid, cleanup);
	tst_tmpdir();
	fd = open(cleanup, testfile, O_CREAT | O_RDWR, 0644);
	tst_sig(FORK, DEF_HANDLER, cleanup);
	TEST_PAUSE;
}

void cleanup(void)
{
	close(fd);
	tst_rmdir();
}

