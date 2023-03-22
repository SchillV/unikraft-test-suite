#include "incl.h"
#define QUEUE_NAME	"/test_mqueue"
#define QUEUE_INIT	"/init_mqueue"

uid_t euid;

struct passwd *pw;

char *qname;

struct rlimit rlim;

mqd_t fd, fd2;

mqd_t fd3 = -1;

int max_queues;
struct test_case {
	const char *desc;
	char *qname;
	int oflag;
	struct mq_attr *rq;
	int ret;
	int err;
	void (*setup)(void);
	void (*cleanup)(void);
};
#define PROC_MAX_QUEUES "/proc/sys/fs/mqueue/queues_max"

void create_queue(void);

void unlink_queue(void);

void set_rlimit(void);

void restore_rlimit(void);

void set_max_queues(void);

void restore_max_queues(void);

struct test_case tcase[] = {
	{
		.desc = "NORMAL",
		.qname = QUEUE_NAME,
		.oflag = O_CREAT,
		.rq = &(struct mq_attr){.mq_maxmsg = 20, .mq_msgsize = 16384},
		.ret = 0,
		.err = 0,
	},
	{
		.desc = "NORMAL",
		.qname = QUEUE_NAME,
		.oflag = O_CREAT,
		.ret = 0,
		.err = 0,
	},
	{
		.desc = "NORMAL",
		.qname = "/caaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaa"
			"aaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaa"
			"aaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaa"
			"aaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaa"
			"aaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaa"
			"aaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaa"
			"aaaaaaaaaaaaaaa",
		.oflag = O_CREAT,
		.ret = 0,
		.err = 0,
	},
	{
		.desc = "NORMAL",
		.qname = "/aaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaa"
			"aaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaa"
			"aaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaa"
			"aaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaa"
			"aaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaa"
			"aaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaa"
			"aaaaaaaaaaaaaaaa",
		.oflag = O_CREAT,
		.ret = -1,
		.err = ENAMETOOLONG,
	},
	{
		.desc = "NORMAL",
		.qname = "",
		.oflag = O_CREAT,
		.ret = -1,
		.err = EINVAL,
	},
	{
		.desc = "NORMAL",
		.qname = QUEUE_NAME,
		.ret = -1,
		.err = EACCES,
		.setup = create_queue,
		.cleanup = unlink_queue,
	},
	{
		.desc = "NORMAL",
		.qname = QUEUE_NAME,
		.oflag = O_CREAT | O_EXCL,
		.ret = -1,
		.err = EEXIST,
		.setup = create_queue,
		.cleanup = unlink_queue,
	},
	{
		.desc = "NO_FILE",
		.qname = QUEUE_NAME,
		.oflag = O_CREAT,
		.ret = -1,
		.err = EMFILE,
		.setup = set_rlimit,
		.cleanup = restore_rlimit,
	},
	{
		.desc = "NORMAL",
		.qname = "/notexist",
		.oflag = 0,
		.ret = -1,
		.err = ENOENT,
	},
	{
		.desc = "NO_SPACE",
		.qname = QUEUE_NAME,
		.oflag = O_CREAT,
		.ret = -1,
		.err = ENOSPC,
		.setup = set_max_queues,
		.cleanup = restore_max_queues,
	}
};

void create_queue(void)
{
	fd2 = mq_open(QUEUE_NAME, O_CREAT | O_EXCL | O_RDWR, S_IRWXU, NULL);
	seteuid(pw->pw_uid);
}

void unlink_queue(void)
{
	seteuid(euid);
	if (fd2 > 0)
		close(fd2);
	if (mq_unlink(QUEUE_NAME))
		tst_brk(TBROK | TERRNO, "mq_close(" QUEUE_NAME ") failed");
}

void set_max_queues(void)
{
	file_scanf(PROC_MAX_QUEUES, "%d", &max_queues);
	fprintf(PROC_MAX_QUEUES, "%d", 1);
	seteuid(pw->pw_uid);
}

void restore_max_queues(void)
{
	seteuid(euid);
	fprintf(PROC_MAX_QUEUES, "%d", max_queues);
}

void set_rlimit(void)
{
	if (rlim.rlim_cur > 0) {
		struct rlimit r;
		r.rlim_cur = 0;
		r.rlim_max = rlim.rlim_max;
		setrlimit(RLIMIT_NOFILE, &r);
	}
}

void restore_rlimit(void)
{
	setrlimit(RLIMIT_NOFILE, &rlim);
}

void setup(void)
{
	euid = geteuid();
	pw = getpwnam("nobody");
	getrlimit(RLIMIT_NOFILE, &rlim);
	fd3 = mq_open(QUEUE_INIT, O_CREAT | O_EXCL | O_RDWR, S_IRWXU, NULL);
}

void cleanup(void)
{
	if (fd > 0)
		mq_close(fd);
	if (fd2 > 0)
		mq_close(fd2);
	if (fd3 > 0 && mq_close(fd3))
		tst_res(TWARN | TERRNO, "mq_close(%s) failed", QUEUE_INIT);
	if (mq_unlink(QUEUE_INIT))
		tst_res(TWARN | TERRNO, "mq_unlink(%s) failed", QUEUE_INIT);
	mq_unlink(qname);
}

void do_test(unsigned int i)
{
	struct test_case *tc = &tcase[i];
	struct mq_attr oldattr;
	qname = tc->qname;
	fd = fd2 = -1;
	tst_res(TINFO, "queue name \"%s\"", qname);
	if (tc->setup)
		tc->setup();
fd = mq_open(qname, tc->oflag, S_IRWXU, tc->rq);
	if (fd > 0 && tc->rq) {
		if (mq_getattr(fd, &oldattr) < 0) {
			tst_res(TFAIL | TERRNO, "mq_getattr failed");
			goto CLEANUP;
		}
		if (oldattr.mq_maxmsg != tc->rq->mq_maxmsg
			|| oldattr.mq_msgsize != tc->rq->mq_msgsize) {
			tst_res(TFAIL, "wrong mq_attr: "
				"mq_maxmsg expected %ld return %ld, "
				"mq_msgsize expected %ld return %ld",
				tc->rq->mq_maxmsg, oldattr.mq_maxmsg, tc->rq->mq_msgsize,
				oldattr.mq_msgsize);
			goto CLEANUP;
		}
	}
	if (tc->ret == 0) {
		if (TST_RET < 0) {
			tst_res(TFAIL | TTERRNO, "%s wrong return code: %ld",
				tc->desc, TST_RET);
		} else {
			tst_res(TPASS | TTERRNO, "%s returned: %ld",
				tc->desc, TST_RET);
		}
		goto CLEANUP;
	}
	if (TST_ERR != tc->err) {
		tst_res(TFAIL | TTERRNO, "%s expected errno: %d",
			tc->desc, TST_ERR);
		goto CLEANUP;
	}
	if (TST_RET != tc->ret) {
		tst_res(TFAIL | TTERRNO, "%s wrong return code: %ld",
			tc->desc, TST_RET);
	} else {
		tst_res(TPASS | TTERRNO, "%s returned: %ld",
			tc->desc, TST_RET);
	}
CLEANUP:
	if (tc->cleanup)
		tc->cleanup();
	if (TST_RET != -1) {
		if (fd > 0)
			close(fd);
		mq_unlink(qname);
	}
}

void main(){
	setup();
	cleanup();
}
