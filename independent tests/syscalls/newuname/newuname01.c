#include "incl.h"
char *TCID = "newuname01";
int testno;
int TST_TOTAL = 1;
void cleanup(void)
{
	tst_rmdir();
	tst_exit();
}
void setup(void)
{
	TEST_PAUSE;
	tst_tmpdir();
}
int main(int ac, char **av)
{
	struct utsname name;
	int lc;
	tst_parse_opts(ac, av, NULL, NULL);
	setup();
	for (lc = 0; TEST_LOOPING(lc); ++lc) {
		tst_count = 0;
		for (testno = 0; testno < TST_TOTAL; ++testno) {
tst_syscall(__NR_uname, &name);
			if (TEST_RETURN == -1) {
				tst_brkm(TFAIL, cleanup, "%s failed - errno = %d : %s",
					 TCID, TEST_ERRNO,
					 strerror(TEST_ERRNO));
			} else {
				tst_resm(TPASS,
					 "newuname call succeed: return value = %ld ",
					 TEST_RETURN);
				if (TEST_RETURN == 0) {
					tst_resm(TINFO, "This system is %s",
						 name.sysname);
					tst_resm(TINFO,
						 "The system infomation is :");
					tst_resm(TINFO,
						 "System is %s on %s hardware",
						 name.sysname, name.machine);
					tst_resm(TINFO, "Nodename is %s",
						 name.nodename);
					tst_resm(TINFO, "Version is %s, %s",
						 name.release, name.version);
					tst_resm(TINFO, "Domainname is %s ",
						 *(&name.machine + 1));
					cleanup();
					tst_exit();
				} else {
					tst_resm(TFAIL,
						 "%s failed - errno = %d : %s",
						 TCID, TEST_ERRNO,
						 strerror(TEST_ERRNO));
					tst_resm(TINFO,
						 "This system is not Linux");
					cleanup();
					tst_exit();
				}
			}
		}
	}
	tst_exit();
}

