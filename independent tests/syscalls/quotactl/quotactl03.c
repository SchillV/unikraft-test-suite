#include "incl.h"
#define _GNU_SOURCE
#ifdef HAVE_XFS_XQM_H
# include <xfs/xqm.h>

uint32_t test_id = 0xfffffffc;

int  verify_quota(
{
	struct fs_disk_quota res_dquota;
	res_dquota.d_id = 1;
do_quotactl(fd, QCMD(Q_XGETNEXTQUOTA, USRQUOTA), tst_device->de;
		test_id, (void *)&res_dquota));
	if (TST_RET != -1) {
		tst_res(TFAIL, "quotactl() found the next active ID: %u unexpectedly",
				res_dquota.d_id);
		return;
	}
	if (TST_ERR == EINVAL)
		tst_brk(TCONF | TTERRNO,
			"Q_XGETNEXTQUOTA wasn't supported in quotactl()");
	if (TST_ERR != ENOENT)
		tst_res(TFAIL | TTERRNO, "quotactl() failed unexpectedly with %s expected ENOENT",
				tst_strerrno(TST_ERR));
	else
		tst_res(TPASS, "quotactl() failed with ENOENT as expected");
}

void setup(void)
{
	quotactl_info();
	fd = open(MNTPOINT, O_RDONLY);
}

void cleanup(void)
{
	if (fd > -1)
		close(fd);
}

void main(){
	setup();
	cleanup();
}
