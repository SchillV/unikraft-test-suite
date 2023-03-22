#include "incl.h"
  AUTHOR: Barrie Kletscher
  Rewrote :  11-92 by Richard Logan
  CO-PILOT: Dave Baumgartner
  TEST ITEMS:
   1. Check to see if getgroups(-1, gidset) fails and sets errno to EINVAL
   2. Check to see if getgroups(0, gidset) does not return -1 and gidset is
      not modified.
   3. Check to see if getgroups(x, gigset) fails and sets errno to EINVAL,
      where x is one less then what is returned by getgroups(0, gidset).
   4. Check to see if getgroups() succeeds and gidset contains
      group id returned from getgid().
*/

void setup(void);

void cleanup(void);
TCID_DEFINE(getgroups01);
int TST_TOTAL = 4;

GID_T gidset[NGROUPS];

GID_T cmpset[NGROUPS];
int main(int ac, char **av)
{
	int lc;
	GID_T group;
	int i;
	int entries;
	tst_parse_opts(ac, av, NULL, NULL);
	setup();
	for (lc = 0; TEST_LOOPING(lc); lc++) {
		tst_count = 0;
GETGROUPS(cleanup, -1, gidset);
		if (TEST_RETURN == 0) {
			tst_resm(TFAIL, "getgroups succeeded unexpectedly");
		} else {
			if (errno == EINVAL)
				tst_resm(TPASS,
					 "getgroups failed as expected with EINVAL");
			else
				tst_resm(TFAIL | TTERRNO,
					 "getgroups didn't fail as expected with EINVAL");
		}
		 * Check that if ngrps is zero that the number of groups is
		 * return and the the gidset array is not modified.
		 * This is a POSIX special case.
		 */
		memset(gidset, 052, NGROUPS * sizeof(GID_T));
		memset(cmpset, 052, NGROUPS * sizeof(GID_T));
GETGROUPS(cleanup, 0, gidset);
		if (TEST_RETURN == -1) {
			tst_resm(TFAIL | TTERRNO, "getgroups failed unexpectedly");
		} else {
			if (memcmp(cmpset, gidset, NGROUPS * sizeof(GID_T)) != 0)
				tst_resm(TFAIL,
					 "getgroups modified the gidset array");
			else
				tst_resm(TPASS,
					 "getgroups did not modify the gidset "
					 "array");
		}
		 * Check to see that is -1 is returned and errno is set to
		 * EINVAL when ngroups is not big enough to hold all groups.
		 */
		if (TEST_RETURN <= 1) {
			tst_resm(TCONF,
				 "getgroups returned %ld; unable to test that using ngrps >=1 but less than number of grps",
				 TEST_RETURN);
		} else {
GETGROUPS(cleanup, TEST_RETURN - 1, gidset);
			if (TEST_RETURN == -1) {
				if (errno == EINVAL)
					tst_resm(TPASS,
						 "getgroups failed as "
						 "expected with EINVAL");
				else
					tst_resm(TFAIL | TERRNO,
						 "getgroups didn't fail "
						 "with EINVAL");
			} else {
				tst_resm(TFAIL,
					 "getgroups succeeded unexpectedly with %ld",
					 TEST_RETURN);
			}
		}
GETGROUPS(cleanup, NGROUPS, gidset);
		if ((entries = TEST_RETURN) == -1) {
			tst_resm(TFAIL | TTERRNO,
				 "getgroups failed unexpectedly");
		} else {
			group = getgid();
			for (i = 0; i < entries; i++) {
				if (gidset[i] == group) {
					tst_resm(TPASS,
						 "getgroups(NGROUPS,gidset) "
						 "returned %d contains gid %d "
						 "(from getgid)",
						 entries, group);
					break;
				}
			}
			if (i == entries) {
				tst_resm(TFAIL,
					 "getgroups(NGROUPS,gidset) ret %d, does "
					 "not contain gid %d (from getgid)",
					 entries, group);
			}
		}
	}
	cleanup();
	tst_exit();
}

void setup(void)
{
	tst_sig(FORK, DEF_HANDLER, cleanup);
	TEST_PAUSE;
	gid_t init_gidset[3] = {0, 1, 2};
	setgroups(3, init_gidset);
}

void cleanup(void)
{
}

