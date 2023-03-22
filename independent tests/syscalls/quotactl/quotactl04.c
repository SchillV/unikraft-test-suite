#include "incl.h"
#define FMTID QFMT_VFS_V1

int32_t fmt_id = FMTID;

int test_id, mount_flag;

struct dqblk set_dq = {
	.dqb_bsoftlimit = 100,
	.dqb_valid = QIF_BLIMITS
};

struct dqblk res_dq;

struct dqinfo set_qf = {
	.dqi_bgrace = 80,
	.dqi_valid = IIF_BGRACE
};

struct dqinfo res_qf;

int32_t fmt_buf;

struct if_nextdqblk res_ndq;

int getnextquota_nsup;

struct tcase {
	int cmd;
	int *id;
	void *addr;
	void *set_data;
	void *res_data;
	int sz;
	char *des;
	char *tname;
} tcases[] = {
	{QCMD(Q_QUOTAON, PRJQUOTA), &fmt_id, NULL,
	NULL, NULL, 0, "turn on quota for project",
	"QCMD(Q_QUOTAON, PRJQUOTA)"},
	{QCMD(Q_SETQUOTA, PRJQUOTA), &test_id, &set_dq,
	NULL, NULL, 0, "set disk quota limit for project",
	"QCMD(Q_SETQUOTA, PRJQUOTA)"},
	{QCMD(Q_GETQUOTA, PRJQUOTA), &test_id, &res_dq,
	&set_dq.dqb_bsoftlimit, &res_dq.dqb_bsoftlimit,
	sizeof(res_dq.dqb_bsoftlimit), "get disk quota limit for project",
	"QCMD(Q_GETQUOTA, PRJQUOTA)"},
	{QCMD(Q_SETINFO, PRJQUOTA), &test_id, &set_qf,
	NULL, NULL, 0, "set information about quotafile for project",
	"QCMD(Q_SETINFO, PRJQUOTA"},
	{QCMD(Q_GETINFO, PRJQUOTA), &test_id, &res_qf,
	&set_qf.dqi_bgrace, &res_qf.dqi_bgrace, sizeof(res_qf.dqi_bgrace),
	"get information about quotafile for project",
	"QCMD(Q_GETINFO, PRJQUOTA"},
	{QCMD(Q_GETFMT, PRJQUOTA), &test_id, &fmt_buf,
	&fmt_id, &fmt_buf, sizeof(fmt_buf),
	"get quota format for project", "QCMD(Q_GETFMT, PRJQUOTA)"},
	{QCMD(Q_GETNEXTQUOTA, PRJQUOTA), &test_id, &res_ndq,
	&test_id, &res_ndq.dqb_id, sizeof(res_ndq.dqb_id),
	"get next disk quota limit for project",
	"QCMD(Q_GETNEXTQUOTA, PRJQUOTA)"},
	{QCMD(Q_QUOTAOFF, PRJQUOTA), &test_id, NULL,
	NULL, NULL, 0, "turn off quota for project",
	"QCMD(Q_QUOTAOFF, PRJQUOTA)"},
};

void setup(void)
{
	const char *const fs_opts[] = {"-I 256", "-O quota,project", NULL};
	quotactl_info();
	mkfs(tst_device->dev, tst_device->fs_type, fs_opts, NULL);
	mount(tst_device->dev, MNTPOINT, tst_device->fs_type, 0, NULL);
	mount_flag = 1;
	fd = open(MNTPOINT, O_RDONLY);
do_quotactl(fd, QCMD(Q_GETNEXTQUOTA, PRJQUOTA), tst_device->de;
		test_id, (void *) &res_ndq));
	if (TST_ERR == EINVAL || TST_ERR == ENOSYS)
		getnextquota_nsup = 1;
}

void cleanup(void)
{
	if (fd > -1)
		close(fd);
	if (mount_flag && tst_umount(MNTPOINT))
		tst_res(TWARN | TERRNO, "umount(%s)", MNTPOINT);
}

int  verify_quota(unsigned int n)
{
	struct tcase *tc = &tcases[n];
	res_dq.dqb_bsoftlimit = 0;
	res_qf.dqi_igrace = 0;
	fmt_buf = 0;
	tst_res(TINFO, "Test #%d: %s", n, tc->tname);
	if (tc->cmd == QCMD(Q_GETNEXTQUOTA, PRJQUOTA) && getnextquota_nsup) {
		tst_res(TCONF, "current system doesn't support this cmd");
		return;
	}
	TST_EXP_PASS_SILENT(do_quotactl(fd, tc->cmd, tst_device->dev, *tc->id, tc->addr),
			"do_quotactl to %s", tc->des);
	if (!TST_PASS)
		return;
	if (memcmp(tc->res_data, tc->set_data, tc->sz)) {
		tst_res(TFAIL, "quotactl failed to %s", tc->des);
		tst_res_hexd(TINFO, tc->res_data, tc->sz, "retval:   ");
		tst_res_hexd(TINFO, tc->set_data, tc->sz, "expected: ");
		return;
	}
	tst_res(TPASS, "quotactl succeeded to %s", tc->des);
}

void main(){
	setup();
	cleanup();
}
