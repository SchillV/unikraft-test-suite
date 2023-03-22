#include "incl.h"

gid_t gid;

gid_t pre_gid;

const char nobody_uid[] = "nobody";

struct passwd *ltpuser;

void run(unsigned int i)
{
	int cnt;
	GID16_CHECK(gid, setfsgid);
	if (i == 0) {
		ltpuser = getpwnam(nobody_uid);
		seteuid(ltpuser->pw_uid);
	}
	 * Run SETFSGID() twice to check the second running have changed
	 * the gid for privileged user, and have not changed the gid
	 * for unprivileged user.
	 */
	for (cnt = 0; cnt < 2; cnt++) {
SETFSGID(gid);
		if ((long)pre_gid != TST_RET) {
			tst_res(TFAIL, "EUID %d: setfsgid() returned %ld", geteuid(), TST_RET);
		} else {
			tst_res(TPASS, "EUID %d: setfsgid() returned expected value: %ld",
				geteuid(), TST_RET);
			if (i == 1) {
				pre_gid = gid;
				gid++;
			}
		}
	}
	if (i == 0)
		seteuid(0);
}

void setup(void)
{
	pre_gid = 0;
	gid = 1;
}

void main(){
	setup();
	cleanup();
}
