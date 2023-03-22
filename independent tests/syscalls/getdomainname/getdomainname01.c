#include "incl.h"
#define MAX_NAME_LEN __NEW_UTS_LEN

int  verify_getdomainname(
{
	char domain_name[MAX_NAME_LEN];
	TST_EXP_PASS(getdomainname(domain_name, sizeof(domain_name)));
}

