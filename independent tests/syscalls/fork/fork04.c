#include "incl.h"
char *TCID = "fork04";
#define	KIDEXIT	42
#define MAX_LINE_LENGTH 256
#define OUTPUT_FILE  "env.out"
#define ENV_NOT_SET  "getenv() does not find variable set"
char *environ_list[] = { "TERM", "NoTSetzWq", "TESTPROG" };
#define NUMBER_OF_ENVIRON (sizeof(environ_list)/sizeof(char *))
int TST_TOTAL = NUMBER_OF_ENVIRON;

void cleanup(void)
{
	tst_rmdir();
}

void setup(void)
{
	tst_sig(FORK, DEF_HANDLER, cleanup);
	TEST_PAUSE;
	tst_tmpdir();
	putenv("TESTPROG=FRKTCS04");
}

void child_environment(void)
{
	int fildes;
	int index;
	char msg[MAX_LINE_LENGTH];
	char *var;
	fildes = creat(OUTPUT_FILE, 0700);
	for (index = 0; index < (int)NUMBER_OF_ENVIRON; index++) {
		memset(msg, 0, MAX_LINE_LENGTH);
		var = getenv(environ_list[index]);
		if (var == NULL)
			(void)sprintf(msg, "%s:%s", environ_list[index],
				      ENV_NOT_SET);
		else
			(void)sprintf(msg, "%s:%s", environ_list[index], var);
		write(fildes, msg, sizeof(msg));
	}
	close(fildes);
}

int cmp_env_strings(char *pstring, char *cstring)
{
	char *penv, *cenv, *pvalue, *cvalue;
	 * Break pstring into env and value
	 */
	penv = pstring;
	pvalue = strchr(pstring, ':');
	if (pvalue == NULL) {
		tst_resm(TBROK,
			 "internal error - parent's env string not in correct format:'%s'",
			 pstring);
		return -1;
	} else {
		*pvalue = '\0';
		pvalue++;
		if (*pvalue == '\0') {
			tst_resm(TBROK,
				 "internal error - missing parent's env value");
			return -1;
		}
	}
	 * Break cstring into env and value
	 */
	cenv = cstring;
	cvalue = strchr(cstring, ':');
	if (cvalue == NULL) {
		tst_resm(TBROK,
			 "internal error - parent's env string not in correct format:'%s'",
			 cstring);
		return -1;
	} else {
		*cvalue = '\0';
		cvalue++;
		if (*cvalue == '\0') {
			tst_resm(TBROK,
				 "internal error - missing child's env value");
			return -1;
		}
	}
	if (strcmp(penv, cenv) != 0) {
		tst_resm(TBROK, "internal error - parent(%s) != child (%s) env",
			 penv, cenv);
		return -1;
	}
	if (strcmp(pvalue, cvalue) != 0) {
		tst_resm(TFAIL,
			 "Env var %s changed after fork(), parent's %s, child's %s",
			 penv, pvalue, cvalue);
	} else {
		tst_resm(TPASS, "Env var %s unchanged after fork(): %s",
			 penv, cvalue);
	}
	return 0;
}
void parent_environment(void)
{
	int fildes;
	char tmp_line[MAX_LINE_LENGTH];
	char parent_value[MAX_LINE_LENGTH];
	unsigned int index;
	int ret;
	char *var;
	fildes = open(cleanup, OUTPUT_FILE, O_RDWR);
	for (index = 0; index < NUMBER_OF_ENVIRON; index++) {
		ret = read(fildes, tmp_line, MAX_LINE_LENGTH);
		if (ret == 0) {
			tst_resm(TBROK,
				 "fork() test. parent_environment: failed to read from file with %d (%s)",
				 errno, strerror(errno));
		} else {
			var = getenv(environ_list[index]);
			if (var == NULL)
				sprintf(parent_value, "%s:%s",
					environ_list[index], ENV_NOT_SET);
			else
				sprintf(parent_value, "%s:%s",
					environ_list[index], var);
			cmp_env_strings(parent_value, tmp_line);
		}
	}
	close(fildes);
}
int main(int ac, char **av)
{
	int lc;
	int kid_status;
	int wait_status;
	int fails;
	tst_parse_opts(ac, av, NULL, NULL);
	setup();
	for (lc = 0; TEST_LOOPING(lc); lc++) {
		tst_count = 0;
		fails = 0;
fork();
		if (TEST_RETURN == -1) {
			tst_brkm(TFAIL, cleanup,
				 "fork() failed with %d (%s)",
				 TEST_ERRNO, strerror(TEST_ERRNO));
		} else if (TEST_RETURN == 0) {
			child_environment();
			exit(KIDEXIT);
		} else {
			wait_status = waitpid(TEST_RETURN, &kid_status, 0);
			if (wait_status == TEST_RETURN) {
				if (kid_status != KIDEXIT << 8) {
					tst_brkm(TBROK, cleanup,
						 "fork(): Incorrect child status returned on wait(): %d",
						 kid_status);
					fails++;
				}
			} else {
				tst_brkm(TBROK, cleanup,
					 "fork(): wait() for child status failed with %d errno: %d : %s",
					 wait_status, errno,
					 strerror(errno));
				fails++;
			}
			if (fails == 0) {
				parent_environment();
			}
		}
	}
	cleanup();
	tst_exit();
}

