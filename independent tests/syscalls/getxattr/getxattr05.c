#include "incl.h"
#define _GNU_SOURCE
#ifdef HAVE_SYS_XATTR_H
# include <sys/xattr.h>
#endif
#ifdef HAVE_LIBACL
# include <sys/acl.h>
#endif
#if defined(HAVE_SYS_XATTR_H) && defined(HAVE_LIBACL)
#define TEST_FILE	"testfile"
#define SELF_USERNS	"/proc/self/ns/user"
#define MAX_USERNS	"/proc/sys/user/max_user_namespaces"
#define UID_MAP	"/proc/self/uid_map"

acl_t acl;

int orig_max_userns = -1;

int user_ns_supported = 1;

struct tcase {
	int set_userns;
	int map_root;
} tcases[] = {
	{0, 0},
	{1, 0},
	{1, 1},
};

int  verify_getxattr(
{
	ssize_t i, res1, res2;
	char buf1[128], buf2[132];
	res1 = getxattr(TEST_FILE, "system.posix_acl_access",
			     buf1, sizeof(buf1));
	res2 = getxattr(TEST_FILE, "system.posix_acl_access",
			     buf2, sizeof(buf2));
	if (res1 != res2) {
		tst_res(TFAIL, "Return different sizes when acquiring "
			"the value of system.posix_acl_access twice");
		return;
	}
	for (i = 0; i < res1; i++) {
		if (buf1[i] != buf2[i])
			break;
	}
	if (i < res1) {
		tst_res(TFAIL, "Got different data(%02x != %02x) at %ld",
			buf1[i], buf2[i], i);
		return;
	}
	tst_res(TPASS, "Got same data when acquiring the value of "
		"system.posix_acl_access twice");
}

void do_unshare(int map_root)
{
	int res;
	res = unshare(CLONE_NEWUSER);
	if (res == -1)
		tst_brk(TFAIL | TERRNO, "unshare(CLONE_NEWUSER) failed");
	if (map_root) {
		 * it is available on Linux 3.5
		 */
		access(UID_MAP, F_OK);
		fprintf(UID_MAP, "%d %d %d", 0, 0, 1);
	}
}

void do_getxattr(unsigned int n)
{
	struct tcase *tc = &tcases[n];
	pid_t pid;
	if (tc->set_userns && !user_ns_supported) {
		tst_res(TCONF, "user namespace not available");
		return;
	}
	pid = fork();
	if (!pid) {
		if (tc->set_userns)
			do_unshare(tc->map_root);
int 		verify_getxattr();
		exit(0);
	}
	tst_reap_children();
}

void setup(void)
{
	const char *acl_text = "u::rw-,u:root:rwx,g::r--,o::r--,m::rwx";
	int res;
	touch(TEST_FILE, 0644, NULL);
	acl = acl_from_text(acl_text);
	if (!acl)
		tst_brk(TBROK | TERRNO, "acl_from_text() failed");
	res = acl_set_file(TEST_FILE, ACL_TYPE_ACCESS, acl);
	if (res == -1) {
		if (errno == EOPNOTSUPP)
			tst_brk(TCONF | TERRNO, "acl_set_file()");
		tst_brk(TBROK | TERRNO, "acl_set_file(%s) failed", TEST_FILE);
	}
	 * We need to change the default value to call unshare().
	 */
	if (access(SELF_USERNS, F_OK) != 0) {
		user_ns_supported = 0;
	} else if (!access(MAX_USERNS, F_OK)) {
		file_scanf(MAX_USERNS, "%d", &orig_max_userns);
		fprintf(MAX_USERNS, "%d", 10);
	}
}

void cleanup(void)
{
	if (orig_max_userns != -1)
		fprintf(MAX_USERNS, "%d", orig_max_userns);
	if (acl)
		acl_free(acl);
}

void main(){
	setup();
	cleanup();
}
