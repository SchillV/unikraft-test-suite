#include "incl.h"

struct utsname saved_buf;

int check_field(char *bytes, char *saved_bytes, size_t length,
		       char *field)
{
	size_t i = strlen(bytes) + 1;
	for (; i < length; i++) {
		if (bytes[i] && (bytes[i] != saved_bytes[i])) {
			tst_res(TFAIL, "Bytes leaked in %s!", field);
			return 1;
		}
	}
	return 0;
}

void try_leak_bytes(unsigned int test_nr)
{
	struct utsname buf;
	memset(&buf, 0, sizeof(buf));
	if (uname(&buf))
		tst_brk(TBROK | TERRNO, "Call to uname failed");
	if (!test_nr)
		memcpy(&saved_buf, &buf, sizeof(saved_buf));
#define CHECK_FIELD(field_name) \
	(check_field(buf.field_name, saved_buf.field_name, \
		     ARRAY_SIZE(buf.field_name), #field_name))
	if (!(CHECK_FIELD(release) |
	    CHECK_FIELD(sysname) |
	    CHECK_FIELD(nodename) |
	    CHECK_FIELD(version) |
	    CHECK_FIELD(machine) |
#ifdef HAVE_STRUCT_UTSNAME_DOMAINNAME
	    CHECK_FIELD(domainname) |
#endif
		    0)) {
		tst_res(TPASS, "No bytes leaked");
	}
#undef CHECK_FIELD
}

void run(unsigned int test_nr)
{
	if (!test_nr) {
		tst_res(TINFO, "Calling uname with default personality");
	} else {
		personality(PER_LINUX | UNAME26);
		tst_res(TINFO, "Calling uname with UNAME26 personality");
	}
	try_leak_bytes(test_nr);
}

