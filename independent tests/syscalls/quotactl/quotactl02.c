#include "incl.h"
#ifdef HAVE_XFS_XQM_H

uint32_t qflagu = XFS_QUOTA_UDQ_ENFD;

uint32_t qflagg = XFS_QUOTA_GDQ_ENFD;

struct t_case {
	int cmd;
	void *addr;
	void (*func_check)();
	int check_subcmd;
	int flag;
	char *des;
	char *tname;
} tcases[] = {
	{QCMD(Q_XQUOTAOFF, USRQUOTA), &qflagu, check_qoff,
	QCMD(Q_XGETQSTAT, USRQUOTA), 1,
	"turn off xfs quota and get xfs quota off status for user",
	"QCMD(Q_XGETQSTAT, USRQUOTA) off"},
	{QCMD(Q_XQUOTAON, USRQUOTA), &qflagu, check_qon,
	QCMD(Q_XGETQSTAT, USRQUOTA), 1,
	"turn on xfs quota and get xfs quota on status for user",
	"QCMD(Q_XGETQSTAT, USRQUOTA) on"},
	{QCMD(Q_XSETQLIM, USRQUOTA), &set_dquota, check_qlim,
	QCMD(Q_XGETQUOTA, USRQUOTA), 0, "Q_XGETQUOTA for user",
	"QCMD(Q_XGETQUOTA, USRQUOTA) qlim"},
	{QCMD(Q_XSETQLIM, USRQUOTA), &set_dquota, check_qlim,
	QCMD(Q_XGETNEXTQUOTA, USRQUOTA), 0, "Q_XGETNEXTQUOTA for user",
	"QCMD(Q_XGETNEXTQUOTA, USRQUOTA)"},
	{QCMD(Q_XQUOTAOFF, USRQUOTA), &qflagu, check_qoffv,
	QCMD(Q_XGETQSTATV, USRQUOTA), 1,
	"turn off xfs quota and get xfs quota off statv for user",
	"QCMD(Q_XGETQSTATV, USRQUOTA) off"},
	{QCMD(Q_XQUOTAON, USRQUOTA), &qflagu, check_qonv,
	QCMD(Q_XGETQSTATV, USRQUOTA), 1,
	"turn on xfs quota and get xfs quota on statv for user",
	"QCMD(Q_XGETQSTATV, USRQUOTA) on"},
	{QCMD(Q_XQUOTAOFF, GRPQUOTA), &qflagg, check_qoff,
	QCMD(Q_XGETQSTAT, GRPQUOTA), 1,
	"turn off xfs quota and get xfs quota off status for group",
	"QCMD(Q_XGETQSTAT, GRPQUOTA) off"},
	{QCMD(Q_XQUOTAON, GRPQUOTA), &qflagg, check_qon,
	QCMD(Q_XGETQSTAT, GRPQUOTA), 1,
	"turn on xfs quota and get xfs quota on status for group",
	"QCMD(Q_XGETQSTAT, GRPQUOTA) on"},
	{QCMD(Q_XSETQLIM, GRPQUOTA), &set_dquota, check_qlim,
	QCMD(Q_XGETQUOTA, GRPQUOTA), 0, "Q_XGETQUOTA for group",
	"QCMD(Q_XGETQUOTA, GRPQUOTA) qlim"},
	{QCMD(Q_XSETQLIM, GRPQUOTA), &set_dquota, check_qlim,
	QCMD(Q_XGETNEXTQUOTA, GRPQUOTA), 0, "Q_XGETNEXTQUOTA for group",
	"QCMD(Q_XGETNEXTQUOTA, GRPQUOTA)"},
	{QCMD(Q_XQUOTAOFF, GRPQUOTA), &qflagg, check_qoffv,
	QCMD(Q_XGETQSTATV, GRPQUOTA), 1,
	"turn off xfs quota and get xfs quota off statv for group",
	"QCMD(Q_XGETQSTATV, GRPQUOTA) off"},
	{QCMD(Q_XQUOTAON, GRPQUOTA), &qflagg, check_qonv,
	QCMD(Q_XGETQSTATV, GRPQUOTA), 1,
	"turn on xfs quota and get xfs quota on statv for group",
	"QCMD(Q_XGETQSTATV, GRPQUOTA) on"},
};

void setup(void)
{
	quotactl_info();
	fd = open(MNTPOINT, O_RDONLY);
	check_support_cmd(USRQUOTA);
	check_support_cmd(GRPQUOTA);
}

void cleanup(void)
{
	if (fd > -1)
		close(fd);
}

int  verify_quota(unsigned int n)
{
	struct t_case *tc = &tcases[n];
	tst_res(TINFO, "Test #%d: %s", n, tc->tname);
	if ((tc->check_subcmd == QCMD(Q_XGETNEXTQUOTA, USRQUOTA)
			|| tc->check_subcmd == QCMD(Q_XGETNEXTQUOTA, GRPQUOTA))
			&& x_getnextquota_nsup) {
		tst_res(TCONF, "current system doesn't support this cmd");
		return;
	}
	if ((tc->check_subcmd == QCMD(Q_XGETQSTATV, USRQUOTA)
			|| tc->check_subcmd == QCMD(Q_XGETQSTATV, GRPQUOTA))
			&& x_getstatv_nsup) {
		tst_res(TCONF, "current system doesn't support this cmd");
		return;
	}
	TST_EXP_PASS_SILENT(do_quotactl(fd, tc->cmd, tst_device->dev, test_id, tc->addr),
		"do_quotactl()");
	if (!TST_PASS)
		return;
	if (tc->flag)
		tc->func_check(tc->check_subcmd, tc->des, *(int *)(tc->addr));
	else
		tc->func_check(tc->check_subcmd, tc->des);
}

void main(){
	setup();
	cleanup();
}
