#include "incl.h"
TCID_DEFINE(setreuid07);
int TST_TOTAL = 1;

char testfile[256] = "";

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
	int lc;
	int pid;
	int status;
	for (lc = 0; TEST_LOOPING(lc); lc++) {
		int tst_fd;
		tst_count = 0;
		if (SETREUID(NULL, 0, ltpuser->pw_uid) == -1) {
			perror("setreuid failed");
			exit(TFAIL);
		}
		 *         with RDWR permissions.
		 */
tst_fd = open(testfile, O_RDWR);
		if (TEST_RETURN != -1) {
			printf("open succeeded unexpectedly\n");
			close(tst_fd);
			exit(TFAIL);
		}
		if (TEST_ERRNO == EACCES) {
			printf("open failed with EACCES as expected\n");
		} else {
			perror("open failed unexpectedly");
			exit(TFAIL);
		}
		 *         with RDWR permissions.
		 */
		pid = FORK_OR_VFORK();
		if (pid < 0)
			tst_brkm(TBROK, cleanup, "Fork failed");
		if (pid == 0) {
			int tst_fd2;
tst_fd2 = open(testfile, O_RDWR);
			if (TEST_RETURN != -1) {
				printf("call succeeded unexpectedly\n");
				close(tst_fd2);
				exit(TFAIL);
			}
			if (TEST_ERRNO == EACCES) {
				printf("open failed with EACCES as expected\n");
				exit(TPASS);
			} else {
				printf("open failed unexpectedly\n");
				exit(TFAIL);
			}
		} else {
			if (waitpid(pid, &status, 0) == -1) {
				perror("waitpid failed");
				exit(TFAIL);
			}
			if (!WIFEXITED(status) || (WEXITSTATUS(status) != 0))
				exit(WEXITSTATUS(status));
		}
		 *         the file with RDWR permissions.
		 */
		tst_count++;
		if (SETREUID(NULL, 0, 0) == -1) {
			perror("setreuid failed");
			exit(TFAIL);
		}
tst_fd = open(testfile, O_RDWR);
		if (TEST_RETURN == -1) {
			perror("open failed unexpectedly");
			exit(TFAIL);
		} else {
			printf("open call succeeded\n");
			close(tst_fd);
		}
	}
	exit(TPASS);
}

void setup(void)
{
	tst_require_root();
	ltpuser = getpwnam("nobody");
	if (ltpuser == NULL)
		tst_brkm(TBROK, NULL, "nobody must be a valid user.");
	tst_tmpdir();
	sprintf(testfile, "setreuid07file%d.tst", getpid());
	fd = open(cleanup, testfile, O_CREAT | O_RDWR, 0644);
	tst_sig(FORK, DEF_HANDLER, cleanup);
	TEST_PAUSE;
}

void cleanup(void)
{
	close(fd);
	tst_rmdir();
}

