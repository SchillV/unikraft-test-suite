#include "incl.h"
#define USRPATH MNTPOINT "/aquota.user"
#define GRPPATH MNTPOINT "/aquota.group"
#define MNTPOINT	"mntpoint"

int32_t fmt_id;

int test_id;

char usrpath[] = USRPATH;

char grppath[] = GRPPATH;

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

int getnextquota_nsup;

struct if_nextdqblk res_ndq;

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
	{QCMD(Q_QUOTAON, USRQUOTA), &fmt_id, usrpath,
	NULL, NULL, 0, "turn on quota for user",
	"QCMD(Q_QUOTAON, USRQUOTA)"},
	{QCMD(Q_SETQUOTA, USRQUOTA), &test_id, &set_dq,
	NULL, NULL, 0, "set disk quota limit for user",
	"QCMD(Q_SETQUOTA, USRQUOTA)"},
	{QCMD(Q_GETQUOTA, USRQUOTA), &test_id, &res_dq,
	&set_dq.dqb_bsoftlimit, &res_dq.dqb_bsoftlimit,
	sizeof(res_dq.dqb_bsoftlimit), "get disk quota limit for user",
	"QCMD(Q_GETQUOTA, USRQUOTA)"},
	{QCMD(Q_SETINFO, USRQUOTA), &test_id, &set_qf,
	NULL, NULL, 0, "set information about quotafile for user",
	"QCMD(Q_SETINFO, USRQUOTA)"},
	{QCMD(Q_GETINFO, USRQUOTA), &test_id, &res_qf,
	&set_qf.dqi_bgrace, &res_qf.dqi_bgrace, sizeof(res_qf.dqi_bgrace),
	"get information about quotafile for user",
	"QCMD(Q_GETINFO, USRQUOTA)"},
	{QCMD(Q_GETFMT, USRQUOTA), &test_id, &fmt_buf,
	&fmt_id, &fmt_buf, sizeof(fmt_buf),
	"get quota format for user",
	"QCMD(Q_GETFMT, USRQUOTA)"},
	{QCMD(Q_SYNC, USRQUOTA), &test_id, &res_dq,
	NULL, NULL, 0, "update quota usages for user",
	"QCMD(Q_SYNC, USRQUOTA)"},
	{QCMD(Q_GETNEXTQUOTA, USRQUOTA), &test_id, &res_ndq,
	&test_id, &res_ndq.dqb_id, sizeof(res_ndq.dqb_id),
	"get next disk quota limit for user",
	"QCMD(Q_GETNEXTQUOTA, USRQUOTA)"},
	{QCMD(Q_QUOTAOFF, USRQUOTA), &test_id, usrpath,
	NULL, NULL, 0, "turn off quota for user",
	"QCMD(Q_QUOTAOFF, USRQUOTA)"},
	{QCMD(Q_QUOTAON, GRPQUOTA), &fmt_id, grppath,
	NULL, NULL, 0, "turn on quota for group",
	"QCMD(Q_QUOTAON, GRPQUOTA)"},
	{QCMD(Q_SETQUOTA, GRPQUOTA), &test_id, &set_dq,
	NULL, NULL, 0, "set disk quota limit for group",
	"QCMD(Q_SETQUOTA, GRPQUOTA)"},
	{QCMD(Q_GETQUOTA, GRPQUOTA), &test_id, &res_dq, &set_dq.dqb_bsoftlimit,
	&res_dq.dqb_bsoftlimit, sizeof(res_dq.dqb_bsoftlimit),
	"set disk quota limit for group",
	"QCMD(Q_GETQUOTA, GRPQUOTA)"},
	{QCMD(Q_SETINFO, GRPQUOTA), &test_id, &set_qf,
	NULL, NULL, 0, "set information about quotafile for group",
	"QCMD(Q_SETINFO, GRPQUOTA)"},
	{QCMD(Q_GETINFO, GRPQUOTA), &test_id, &res_qf, &set_qf.dqi_bgrace,
	&res_qf.dqi_bgrace, sizeof(res_qf.dqi_bgrace),
	"get information about quotafile for group",
	"QCMD(Q_GETINFO, GRPQUOTA)"},
	{QCMD(Q_GETFMT, GRPQUOTA), &test_id, &fmt_buf,
	&fmt_id, &fmt_buf, sizeof(fmt_buf), "get quota format for group",
	"QCMD(Q_GETFMT, GRPQUOTA)"},
	{QCMD(Q_SYNC, GRPQUOTA), &test_id, &res_dq,
	NULL, NULL, 0, "update quota usages for group",
	"QCMD(Q_SYNC, GRPQUOTA)"},
	{QCMD(Q_GETNEXTQUOTA, GRPQUOTA), &test_id, &res_ndq,
	&test_id, &res_ndq.dqb_id, sizeof(res_ndq.dqb_id),
	"get next disk quota limit for group",
	"QCMD(Q_GETNEXTQUOTA, GRPQUOTA)"},
	{QCMD(Q_QUOTAOFF, GRPQUOTA), &test_id, grppath,
	NULL, NULL, 0, "turn off quota for group",
	"QCMD(Q_QUOTAOFF, GRPQUOTA)"},
};

void setup(void)
{
	const struct quotactl_fmt_variant *var = &fmt_variants[tst_variant];
	const char *const cmd[] = {"quotacheck", "-ugF", var->fmt_name, MNTPOINT, NULL};
	tst_res(TINFO, "quotactl() with %s format", var->fmt_name);
	cmd(cmd, NULL, NULL);
	fmt_id = var->fmt_id;
	access(USRPATH, F_OK);
	access(GRPPATH, F_OK);
quotactl(QCMD(Q_GETNEXTQUOTA, USRQUOTA), tst_device->de;
		test_id, (void *) &res_ndq));
	if (TST_ERR == EINVAL || TST_ERR == ENOSYS)
		getnextquota_nsup = 1;
}

void cleanup(void)
{
	unlink(USRPATH);
	unlink(GRPPATH);
}

int  verify_quota(unsigned int n)
{
	struct tcase *tc = &tcases[n];
	res_dq.dqb_bsoftlimit = 0;
	res_qf.dqi_igrace = 0;
	fmt_buf = 0;
	res_ndq.dqb_id = -1;
	tst_res(TINFO, "Test #%d: %s", n, tc->tname);
	if ((tc->cmd == QCMD(Q_GETNEXTQUOTA, USRQUOTA) ||
		tc->cmd == QCMD(Q_GETNEXTQUOTA, GRPQUOTA)) &&
		getnextquota_nsup) {
		tst_res(TCONF, "current system doesn't support this cmd");
		return;
	}
	TST_EXP_PASS_SILENT(quotactl(tc->cmd, tst_device->dev, *tc->id, tc->addr),
			"quotactl to %s", tc->des);
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
