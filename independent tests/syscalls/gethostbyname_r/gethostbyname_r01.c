#include "incl.h"
#define CANARY "in_the_coal_mine"

void setup(void);

void check_vulnerable(void);

struct {
	char buffer[1024];
	char canary[sizeof(CANARY)];
} temp = {
	"buffer",
	CANARY,
};
char *TCID = "gethostbyname_r01";
int TST_TOTAL = 1;
int main(int ac, char **av)
{
	int lc;
	tst_parse_opts(ac, av, NULL, NULL);
	setup();
	for (lc = 0; TEST_LOOPING(lc); lc++) {
		tst_count = 0;
		check_vulnerable();
	}
	tst_exit();
}

void setup(void)
{
	tst_sig(NOFORK, DEF_HANDLER, NULL);
	TEST_PAUSE;
}

void check_vulnerable(void)
{
	struct hostent resbuf;
	struct hostent *result;
	int herrno;
	int retval;
	char name[sizeof(temp.buffer)];
	size_t len;
	 * <glibc>/nss/digits_dots.c:
	 * strlen(name) = size_needed - sizeof(*host_addr) -
	 *                sizeof(*h_addr_ptrs) - 1;
	 */
	len = sizeof(temp.buffer) - 16 - 2 * sizeof(char *) - 1;
	memset(name, '0', len);
	name[len] = '\0';
	retval = gethostbyname_r(name, &resbuf, temp.buffer,
				 sizeof(temp.buffer), &result, &herrno);
	if (strcmp(temp.canary, CANARY) != 0) {
		tst_resm(TFAIL, "vulnerable");
		return;
	}
	if (retval == ERANGE) {
		tst_resm(TPASS, "not vulnerable");
		return;
	}
	tst_resm(TFAIL, "gethostbyname_r() returned %s, expected ERANGE",
		 tst_strerrno(retval));
}

