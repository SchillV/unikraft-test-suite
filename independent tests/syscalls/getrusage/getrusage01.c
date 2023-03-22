#include "incl.h"

struct rusage *usage;
struct test_case_t {
	int who;
	char *desc;
} tc[] = {
	{RUSAGE_SELF, "RUSAGE_SELF"},
	{RUSAGE_CHILDREN, "RUSAGE_CHILDREN"},
};

void run(unsigned int i)
{
	TST_EXP_PASS(getrusage(tc[i].who, usage), "getrusage(%s, %p)", tc[i].desc, usage);
}

void main(){
	setup();
	cleanup();
}
