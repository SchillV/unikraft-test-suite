#include "incl.h"
char *TCID = "modify_ldt03";
int TST_TOTAL = 1;
#if defined(__i386__) && defined(HAVE_MODIFY_LDT)
#ifdef HAVE_ASM_LDT_H
# include <asm/ldt.h>
#endif
extern int modify_ldt(int, void *, unsigned long);
#ifdef HAVE_STRUCT_USER_DESC
# define SIZE sizeof(struct user_desc)
#else
# define SIZE 16
#endif

char buf[SIZE];

void cleanup(void);

void setup(void);
int main(int ac, char **av)
{
	int lc;
	tst_parse_opts(ac, av, NULL, NULL);
	setup();
	for (lc = 0; TEST_LOOPING(lc); lc++) {
		tst_count = 0;
modify_ldt(0, buf, SIZE);
		if (TEST_RETURN < 0) {
			tst_resm(TFAIL | TTERRNO,
				 "modify_ldt() failed with errno: %s",
				 strerror(TEST_ERRNO));
		} else {
			tst_resm(TPASS, "modify_ldt() tested success");
		}
	}
	cleanup();
	tst_exit();
}

void setup(void)
{
	tst_sig(NOFORK, DEF_HANDLER, cleanup);
	TEST_PAUSE;
}

void cleanup(void)
{
}
#elif HAVE_MODIFY_LDT
int main(void)
{
	tst_brkm(TCONF,
		 NULL, "modify_ldt is available but not tested on the platform than "
		 "__i386__");
}
int main(void)
{
	tst_resm(TINFO, "modify_ldt03 test only for ix86");
	tst_exit();
}

