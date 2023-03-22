#include "incl.h"
char *TCID = "sockioctl01";
int testno;


struct sockaddr_in sin0, fsin1;

struct ifconf ifc;

struct ifreq ifr;

int sinlen;

int optval;

char buf[8192];

void setup(void);

void setup0(void);

void setup1(void);

void setup2(void);

void setup3(void);

void cleanup(void);

void cleanup0(void);

void cleanup1(void);
struct test_case_t {
	void *arg;
	struct sockaddr *sin;
	int salen;
	void (*setup) (void);
	void (*cleanup) (void);
	char *desc;
} tdat[] = {
	{
	PF_INET, SOCK_STREAM, 0, SIOCATMARK, &optval,
		    (struct sockaddr *)&fsin1, sizeof(fsin1), -1,
		    EBADF, setup0, cleanup0, "bad file descriptor"}
	, {
	PF_INET, SOCK_STREAM, 0, SIOCATMARK, &optval,
		    (struct sockaddr *)&fsin1, sizeof(fsin1), -1,
		    EINVAL, setup0, cleanup0, "not a socket"}
	,
#if !defined(UCLINUX)
	{
	PF_INET, SOCK_STREAM, 0, SIOCATMARK, 0,
		    (struct sockaddr *)&fsin1, sizeof(fsin1), -1,
		    EFAULT, setup1, cleanup1, "invalid option buffer"}
	,
#endif
	{
	PF_INET, SOCK_DGRAM, 0, SIOCATMARK, &optval,
		    (struct sockaddr *)&fsin1, sizeof(fsin1), -1,
		    EINVAL, setup1, cleanup1, "ATMARK on UDP"}
	, {
	PF_INET, SOCK_DGRAM, 0, SIOCGIFCONF, &ifc,
		    (struct sockaddr *)&fsin1, sizeof(fsin1), 0,
		    0, setup2, cleanup1, "SIOCGIFCONF"}
	, {
	PF_INET, SOCK_DGRAM, 0, SIOCGIFFLAGS, &ifr,
		    (struct sockaddr *)&fsin1, sizeof(fsin1), 0,
		    0, setup3, cleanup1, "SIOCGIFFLAGS"}
	, {
	PF_INET, SOCK_DGRAM, 0, SIOCGIFFLAGS, 0,
		    (struct sockaddr *)&fsin1, sizeof(fsin1), -1,
		    EFAULT, setup3, cleanup1, "SIOCGIFFLAGS with invalid ifr"}
	, {
	PF_INET, SOCK_DGRAM, 0, SIOCSIFFLAGS, 0,
		    (struct sockaddr *)&fsin1, sizeof(fsin1), -1,
		    EFAULT, setup3, cleanup1, "SIOCSIFFLAGS with invalid ifr"}
,};
int TST_TOTAL = sizeof(tdat) / sizeof(tdat[0]);
int main(int argc, char *argv[])
{
	int lc;
	tst_parse_opts(argc, argv, NULL, NULL);
	setup();
	for (lc = 0; TEST_LOOPING(lc); ++lc) {
		tst_count = 0;
		for (testno = 0; testno < TST_TOTAL; ++testno) {
			tdat[testno].setup();
ioctl(s, tdat[testno].cmd, tdat[testno].arg);
			if (TEST_RETURN != tdat[testno].retval ||
			    (TEST_RETURN < 0 &&
			     TEST_ERRNO != tdat[testno].experrno)) {
				tst_resm(TFAIL, "%s ; returned"
					 " %ld (expected %d), errno %d (expected"
					 " %d)", tdat[testno].desc,
					 TEST_RETURN, tdat[testno].retval,
					 TEST_ERRNO, tdat[testno].experrno);
			} else {
				tst_resm(TPASS, "%s successful",
					 tdat[testno].desc);
			}
			tdat[testno].cleanup();
		}
	}
	cleanup();
	tst_exit();
}

void setup(void)
{
	TEST_PAUSE;
	sin0.sin_family = AF_INET;
	sin0.sin_port = 0;
	sin0.sin_addr.s_addr = INADDR_ANY;
	tst_tmpdir();
}

void cleanup(void)
{
	tst_rmdir();
}

void setup0(void)
{
	if (tdat[testno].experrno == EBADF) {
	} else {
		unlink("test");
		if ((mknod("test", S_IRWXU | S_IFIFO, 0)) == -1) {
			tst_brkm(TBROK, cleanup, "Could not create test - "
				 "errno: %s", strerror(errno));
		}
		if ((s = open("test", O_RDWR)) == -1) {
			tst_brkm(TBROK, cleanup, "Could not open test - "
				 "errno: %s", strerror(errno));
		}
		 * kernel commit 46ce341b2f176c2611f12ac390adf862e932eb02
		 * changed -EINVAL to -ENOIOCTLCMD, so vfs_ioctl now
		 * returns -ENOTTY.
		 */
		tdat[testno].experrno = ENOTTY;
	}
}

void cleanup0(void)
{
	if (tdat[testno].experrno != EBADF) {
		(void)close(s);
		s = -1;
	}
}

void setup1(void)
{
	s = socket(cleanup, tdat[testno].domain, tdat[testno].type,
			tdat[testno].proto);
	bind(cleanup, s, cleanup, s, struct sockaddr *)&sin0, sizeofsin0));
	sinlen = sizeof(fsin1);
	if (strncmp(tdat[testno].desc, "ATMARK on UDP", 14) == 0)
		tdat[testno].experrno = ENOTTY;
}

void setup2(void)
{
	s = socket(cleanup, tdat[testno].domain, tdat[testno].type,
			tdat[testno].proto);
	ifc.ifc_len = sizeof(buf);
	ifc.ifc_buf = buf;
}

void setup3(void)
{
	setup2();
	ioctl(cleanup, s, SIOCGIFCONF, &ifc);
	ifr = *(struct ifreq *)ifc.ifc_buf;
}

void cleanup1(void)
{
	(void)close(s);
	s = -1;
}

