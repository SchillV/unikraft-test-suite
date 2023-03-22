#include "incl.h"
*  11/20/2002 Port to LTP <robbiew@us.ibm.com>
*  06/30/2001 Port to Linux <nsharoff@us.ibm.com>
#define _XOPEN_SOURCE 700
#define PAIR(name) {_CS_ ## name, #name}

struct test_case_t {
	int value;
	char *name;
} test_cases[] = {
	PAIR(PATH),
	PAIR(GNU_LIBC_VERSION),
	PAIR(GNU_LIBPTHREAD_VERSION),
	PAIR(POSIX_V7_ILP32_OFF32_CFLAGS),
	PAIR(POSIX_V7_ILP32_OFF32_LDFLAGS),
	PAIR(POSIX_V7_ILP32_OFF32_LIBS),
	PAIR(POSIX_V7_ILP32_OFFBIG_CFLAGS),
	PAIR(POSIX_V7_ILP32_OFFBIG_LDFLAGS),
	PAIR(POSIX_V7_ILP32_OFFBIG_LIBS),
	PAIR(POSIX_V7_LP64_OFF64_CFLAGS),
	PAIR(POSIX_V7_LP64_OFF64_LDFLAGS),
	PAIR(POSIX_V7_LP64_OFF64_LIBS),
	PAIR(POSIX_V7_LPBIG_OFFBIG_CFLAGS),
	PAIR(POSIX_V7_LPBIG_OFFBIG_LDFLAGS),
	PAIR(POSIX_V7_LPBIG_OFFBIG_LIBS),
#ifdef _CS_POSIX_V7_THREADS_CFLAGS
	PAIR(POSIX_V7_THREADS_CFLAGS),
#endif
#ifdef _CS_POSIX_V7_THREADS_LDFLAGS
	PAIR(POSIX_V7_THREADS_LDFLAGS),
#endif
	PAIR(POSIX_V7_WIDTH_RESTRICTED_ENVS),
#ifdef _CS_V7_ENV
	PAIR(V7_ENV),
#endif
};

void run(unsigned int i)
{
	char *buf;
	int len;
	TST_EXP_POSITIVE(confstr(test_cases[i].value, NULL, (size_t)0));
	if (!TST_RET)
		return;
	len = TST_RET;
	buf = malloc(len);
confstr(test_cases[i].value, buf, len);
	if (buf[len - 1] != '\0') {
		tst_brk(TFAIL, "confstr: %s, %s", test_cases[i].name,
			tst_strerrno(TST_ERR));
	} else {
		tst_res(TPASS, "confstr %s = '%s'", test_cases[i].name, buf);
	}
	free(buf);
}

void main(){
	setup();
	cleanup();
}
