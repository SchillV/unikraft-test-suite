#include "incl.h"
#define TESTUSER "root"
TCID_DEFINE(getgroups03);
int TST_TOTAL = 1;

int ngroups;

GID_T groups_list[NGROUPS];

GID_T groups[NGROUPS];

int  verify_groups(int ret_ngroups);

void setup(void);

void cleanup(void);
int main(int ac, char **av)
{
	int lc;
	int gidsetsize = NGROUPS;
	tst_parse_opts(ac, av, NULL, NULL);
	setup();
	for (lc = 0; TEST_LOOPING(lc); lc++) {
		tst_count = 0;
GETGROUPS(cleanup, gidsetsize, groups_list);
		if (TEST_RETURN == -1) {
			tst_resm(TFAIL | TTERRNO, "getgroups failed");
			continue;
		}
int 		verify_groups(TEST_RETURN);
	}
	cleanup();
	tst_exit();
}

int readgroups(GID_T groups[NGROUPS])
{
	struct group *grp;
	int ngrps = 0;
	int i;
	int found;
	GID_T g;
	setgrent();
	while ((grp = getgrent()) != 0) {
		for (i = 0; grp->gr_mem[i]; i++) {
			if (strcmp(grp->gr_mem[i], TESTUSER) == 0) {
				groups[ngrps++] = grp->gr_gid;
			}
		}
	}
	   It is unspecified whether the effective group ID of the
	   calling process is included in the returned list.  (Thus,
	   an application should also call getegid(2) and add or
	   remove the resulting value.).  So, add the value here if
	   it's not in.  */
	found = 0;
	g = getegid();
	for (i = 0; i < ngrps; i++) {
		if (groups[i] == g)
			found = 1;
	}
	if (found == 0)
		groups[ngrps++] = g;
	endgrent();
	return ngrps;
}

void setup(void)
{
	tst_require_root();
	tst_sig(NOFORK, DEF_HANDLER, cleanup);
	TEST_PAUSE;
	 * Get the IDs of all the groups of "root"
	 * from /etc/group file
	 */
	ngroups = readgroups(groups);
	 * if the testcase is executed via an ssh session this
	 * testcase will fail. So execute setgroups() before executing
	 * getgroups()
	 */
	if (SETGROUPS(cleanup, ngroups, groups) == -1)
		tst_brkm(TBROK | TERRNO, cleanup, "setgroups failed");
}

int  verify_groups(int ret_ngroups)
{
	int i, j;
	GID_T egid;
	int egid_flag = 1;
	int fflag = 1;
int 	 * Loop through the array to verify the gids
	 * returned by getgroups().
	 * First, compare each element of the array
	 * returned by getgroups() with that read from
	 * group file.
	 */
	for (i = 0; i < ret_ngroups; i++) {
		for (j = 0; j < ngroups; j++) {
			if (groups_list[i] != groups[j]) {
				if (j == ngroups - 1) {
					tst_resm(TFAIL, "getgroups returned "
						 "incorrect gid %d",
						 groups_list[i]);
					fflag = 0;
				} else {
					continue;
				}
			} else {
				break;
			}
		}
	}
	egid = getegid();
	for (i = 0; i < ngroups; i++) {
		for (j = 0; j < ret_ngroups; j++) {
			if (groups[i] != groups_list[j]) {
				 * If the loop ends & gids are not matching
				 * if gid is not egid, exit with error
				 * else egid is returned by getgroups()
				 */
				if (j == (ret_ngroups - 1)) {
					if (groups[i] != egid) {
						tst_resm(TFAIL, "getgroups "
							 "didn't return %d one "
							 "of the gids of %s",
							 groups[i], TESTUSER);
						fflag = 0;
					} else {
						 * egid is not present in
						 * group_list.
						 * Reset the egid flag
						 */
						egid_flag = 0;
					}
				}
			} else {
				break;
			}
		}
	}
	 * getgroups() should return the no. of gids for TESTUSER with
	 * or without egid taken into account.
	 * Decrement ngroups, if egid is not returned by getgroups()
	 * Now, if ngroups matches ret_val, as above comparisons of the array
	 * are successful, this implies that the array contents match.
	 */
	if (egid_flag == 0)
		ngroups--;
	if (ngroups != ret_ngroups) {
		tst_resm(TFAIL,
			 "getgroups(2) returned incorrect no. of gids %d "
			 "(expected %d)", ret_ngroups, ngroups);
		fflag = 0;
	}
	if (fflag)
		tst_resm(TPASS, "getgroups functionality correct");
}

void cleanup(void)
{
}

