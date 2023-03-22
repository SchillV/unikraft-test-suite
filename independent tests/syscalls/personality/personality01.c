#include "incl.h"
char *TCID = "personality01";
#define PAIR(id) {id, #id}
struct personalities {
	unsigned long int pers;
	const char *name;
};
struct personalities pers[] = {
	PAIR(PER_LINUX),
	PAIR(PER_LINUX_32BIT),
	PAIR(PER_SVR4),
	PAIR(PER_SVR3),
	PAIR(PER_SCOSVR3),
	PAIR(PER_OSR5),
	PAIR(PER_WYSEV386),
	PAIR(PER_ISCR4),
	PAIR(PER_BSD),
	PAIR(PER_XENIX),
#if defined(__x86_64__)
	PAIR(PER_LINUX32),
#endif
	PAIR(PER_IRIX32),
	PAIR(PER_IRIXN32),
	PAIR(PER_IRIX64),
	PAIR(PER_RISCOS),
	PAIR(PER_SOLARIS),
	PAIR(PER_UW7),
	PAIR(PER_OSF4),
	PAIR(PER_HPUX),
};
int TST_TOTAL = ARRAY_SIZE(pers);

void do_child(unsigned int i)
{
	int ret;
	ret = personality(pers[i].pers);
	if (ret < 0) {
		tst_resm(TFAIL | TERRNO, "personality(%s) failed", pers[i].name);
		return;
	}
	ret = personality(0xffffffff);
	if ((unsigned long)ret != pers[i].pers) {
		tst_resm(TFAIL,
			 "%s: wrong personality read back %d expected %lu",
			 pers[i].name, ret, pers[i].pers);
		return;
	}
	tst_resm(TPASS, "personality(%s)", pers[i].name);
}

int  verify_personality(unsigned int i)
{
	pid_t pid;
	pid = tst_fork();
	switch (pid) {
	case 0:
		do_child(i);
		tst_exit();
	break;
	case -1:
		tst_brkm(TBROK | TERRNO, NULL, "fork() failed");
	break;
	default:
		tst_record_childstatus(NULL, pid);
	break;
	}
}
int main(int ac, char **av)
{
	int i, lc;
	tst_parse_opts(ac, av, NULL, NULL);
	for (lc = 0; TEST_LOOPING(lc); lc++) {
		for (i = 0; i < TST_TOTAL; i++) {
int 			verify_personality(i);
		}
	}
	tst_exit();
}

