#include "incl.h"
void setup();
void cleanup();
void help();
struct pathconf_args {
	char *define_tag;
	int value;
} args[] = {
	{
	"_PC_LINK_MAX", _PC_LINK_MAX}, {
	"_PC_NAME_MAX", _PC_NAME_MAX}, {
	"_PC_PATH_MAX", _PC_PATH_MAX}, {
	"_PC_PIPE_BUF", _PC_PIPE_BUF}, {
	"_PC_CHOWN_RESTRICTED", _PC_CHOWN_RESTRICTED}, {
	"_PC_NO_TRUNC", _PC_NO_TRUNC}, {
	NULL, 0}
};
char *TCID = "pathconf01";
int TST_TOTAL = ARRAY_SIZE(args);
int i;
int lflag;
char *path;
option_t options[] = {
	{NULL, NULL, NULL}
};
int main(int ac, char **av)
{
	int lc;
	tst_parse_opts(ac, av, options, &help);
	if (!lflag) {
		tst_tmpdir();
		path = tst_get_tmpdir();
	}
     * perform global setup for test
     ***************************************************************/
	setup();
     * check looping state if -c option given
     ***************************************************************/
	for (lc = 0; TEST_LOOPING(lc); lc++) {
		tst_count = 0;
		for (i = 0; i < TST_TOTAL; i++) {
			errno = -4;
			 * Call pathconf(2)
			 */
pathconf(path, args[i].value);
			 * A test case can only fail if -1 is returned and the errno
			 * was set.  If the errno remains unchanged, the
			 * system call did not fail.
			 */
			if (TEST_RETURN == -1 && errno != -4) {
				tst_resm(TFAIL,
					 "pathconf(%s, %s) Failed, errno=%d : %s",
					 path, args[i].define_tag, TEST_ERRNO,
					 strerror(TEST_ERRNO));
			} else {
				tst_resm(TPASS,
					 "pathconf(%s, %s) returned %ld",
					 path, args[i].define_tag,
					 TEST_RETURN);
			}
		}
	}
     * cleanup and exit
     ***************************************************************/
	cleanup();
	tst_exit();
}
void setup(void)
{
	tst_sig(NOFORK, DEF_HANDLER, cleanup);
	TEST_PAUSE;
}
void cleanup(void)
{
	if (!lflag) {
		tst_rmdir();
		free(path);
	}
}
void help(void)
{
	printf("  -l path a path to test with pathconf(2) (def: /tmp)\n");
}

