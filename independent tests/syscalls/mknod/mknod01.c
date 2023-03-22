#include "incl.h"

void setup(void);

void cleanup(void);
char *TCID = "mknod01";
#define PATH "test_node"
};
int TST_TOTAL = ARRAY_SIZE(tcases);
int main(int ac, char **av)
{
	int lc, i;
	dev_t dev;
	tst_parse_opts(ac, av, NULL, NULL);
	setup();
	for (lc = 0; TEST_LOOPING(lc); lc++) {
		tst_count = 0;
		for (i = 0; i < TST_TOTAL; i++) {
			 * overlayfs doesn't support mknod char device with
			 * major 0 and minor 0, which is known as whiteout_dev
			 */
			if (S_ISCHR(tcases[i]))
				dev = makedev(1, 3);
			else
				dev = 0;
mknod(PATH, tcases[i], dev);
			if (TEST_RETURN == -1) {
				tst_resm(TFAIL,
					 "mknod(%s, %#o, %lu) failed, errno=%d : %s",
					 PATH, tcases[i], dev, TEST_ERRNO,
					 strerror(TEST_ERRNO));
			} else {
				tst_resm(TPASS,
					 "mknod(%s, %#o, %lu) returned %ld",
					 PATH, tcases[i], dev, TEST_RETURN);
			}
			unlink(cleanup, PATH);
		}
	}
	cleanup();
	tst_exit();
}
void setup(void)
{
	tst_require_root();
	tst_sig(NOFORK, DEF_HANDLER, cleanup);
	TEST_PAUSE;
	tst_tmpdir();
}
void cleanup(void)
{
	tst_rmdir();
}

