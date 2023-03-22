#include "incl.h"

struct sockaddr_in server_addr;

struct sockaddr_in fsin1;

socklen_t sinlen;

socklen_t invalid_sinlen = -1;

int sv[2];

void setup(void);

void setup2(int);

void setup3(int);

void setup4(int);

void cleanup(void);

void cleanup2(int);

void cleanup4(int);
struct test_case_t {
	int sockfd;
	struct sockaddr *sockaddr;
	socklen_t *addrlen;
	int expretval;
	int experrno;
	void (*setup) (int);
	void (*cleanup) (int);
	char *name;
} test_cases[] = {
	{-1, (struct sockaddr *)&fsin1, &sinlen, -1, EBADF, NULL, NULL,
	 "EBADF"},
	{-1, (struct sockaddr *)&fsin1, &sinlen, -1, ENOTSOCK, setup2, cleanup2,
	 "ENOTSOCK"},
	{-1, (struct sockaddr *)&fsin1, &sinlen, -1, ENOTCONN, setup3, cleanup2,
	 "ENOTCONN"},
	{-1, (struct sockaddr *)&fsin1, &invalid_sinlen, -1, EINVAL, setup4,
	 cleanup4, "EINVAL"},
#ifndef UCLINUX
	{-1, (struct sockaddr *)-1, &sinlen, -1, EFAULT, setup4, cleanup4,
	 "EFAULT"},
	{-1, (struct sockaddr *)&fsin1, NULL, -1, EFAULT, setup4,
	 cleanup4, "EFAULT"},
	{-1, (struct sockaddr *)&fsin1, (socklen_t *)1, -1, EFAULT, setup4,
	 cleanup4, "EFAULT"},
#endif
};
char *TCID = "getpeername01";
int TST_TOTAL = ARRAY_SIZE(test_cases);
int main(int argc, char *argv[])
{
	int lc;
	int i;
	tst_parse_opts(argc, argv, NULL, NULL);
	setup();
	for (lc = 0; TEST_LOOPING(lc); ++lc) {
		tst_count = 0;
		for (i = 0; i < TST_TOTAL; ++i) {
			if (test_cases[i].setup != NULL)
				test_cases[i].setup(i);
getpeername(test_cases[i].sockf;
					 test_cases[i].sockaddr,
					 test_cases[i].addrlen));
			if (TEST_RETURN == test_cases[i].expretval &&
			    TEST_ERRNO == test_cases[i].experrno) {
				tst_resm(TPASS,
					 "test getpeername() %s successful",
					 test_cases[i].name);
			} else {
				tst_resm(TFAIL,
					 "test getpeername() %s failed; "
					 "returned %ld (expected %d), errno %d "
					 "(expected %d)", test_cases[i].name,
					 TEST_RETURN, test_cases[i].expretval,
					 TEST_ERRNO, test_cases[i].experrno);
			}
			if (test_cases[i].cleanup != NULL)
				test_cases[i].cleanup(i);
		}
	}
	cleanup();
	tst_exit();
}

void setup(void)
{
	TEST_PAUSE;
	server_addr.sin_family = AF_INET;
	server_addr.sin_port = 0;
	server_addr.sin_addr.s_addr = INADDR_ANY;
	sinlen = sizeof(fsin1);
}

void cleanup(void)
{
}

void setup2(int i)
{
	test_cases[i].sockfd = open(cleanup, "/dev/null", O_WRONLY, 0666);
}

void setup3(int i)
{
	test_cases[i].sockfd = socket(PF_INET, SOCK_STREAM, 0);
	if (test_cases[i].sockfd < 0) {
		tst_brkm(TBROK | TERRNO, cleanup,
			 "socket setup failed for getpeername test %d", i);
	}
	bind(cleanup, test_cases[i].sockfd,
		  (struct sockaddr *)&server_addr, sizeof(server_addr));
}

void setup4(int i)
{
	if (socketpair(PF_UNIX, SOCK_STREAM, 0, sv) < 0) {
		tst_brkm(TBROK | TERRNO, cleanup,
			 "socketpair failed for getpeername test %d", i);
	}
	test_cases[i].sockfd = sv[0];
}

void cleanup2(int i)
{
	close(cleanup, test_cases[i].sockfd);
}

void cleanup4(int i)
{
	close(cleanup, sv[0]);
	close(cleanup, sv[1]);
}

