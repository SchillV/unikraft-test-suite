#include "incl.h"
* Copyright (c) International Business Machines Corp., 2001
*/
* Test Name: socket01
*
* Test Description:
* Verify that socket() returns the proper errno for various failure cases
*
*/
struct test_case_t {
	int domain;
	int type;
	int proto;
	int retval;
	int experrno;
	char *desc;
} tdat[] = {
	{0, SOCK_STREAM, 0, -1, EAFNOSUPPORT, "invalid domain"},
	{PF_INET, 75, 0, -1, EINVAL, "invalid type"},
	{PF_UNIX, SOCK_DGRAM, 0, 0, 0, "UNIX domain dgram"},
	{PF_INET, SOCK_RAW, 0, -1, EPROTONOSUPPORT, "raw open as non-root"},
	{PF_INET, SOCK_DGRAM, 17, 0, 0, "UDP socket"},
	{PF_INET, SOCK_STREAM, 17, -1, EPROTONOSUPPORT, "UDP stream"},
	{PF_INET, SOCK_DGRAM, 6, -1, EPROTONOSUPPORT, "TCP dgram"},
	{PF_INET, SOCK_STREAM, 6, 0, 0, "TCP socket"},
	{PF_INET, SOCK_STREAM, 1, -1, EPROTONOSUPPORT, "ICMP stream"}
};

int  verify_socket(unsigned int n)
{
	int fd;
	struct test_case_t *tc = &tdat[n];
fd = socket(tc->domain, tc->type, tc->proto);
	if (TST_RET >= 0)
		TST_RET = 0;
	if (fd > 0)
		close(fd);
	if (TST_RET != tc->retval) {
		tst_res(TFAIL, "%s returned %d (expected %d)",
			tc->desc, fd, tc->retval);
		return;
	}
	if (TST_ERR != tc->experrno) {
		tst_res(TFAIL | TTERRNO, "expected %s(%d)",
		        tst_strerrno(tc->experrno), tc->experrno);
		return;
	}
	tst_res(TPASS, "%s successful", tc->desc);
}

