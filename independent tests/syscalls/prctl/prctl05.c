#include "incl.h"

struct tcase {
	char setname[20];
	char expname[20];
} tcases[] = {
	{"prctl05_test", "prctl05_test"},
	{"prctl05_test_xxxxx", "prctl05_test_xx"}
};

int  verify_prctl(unsigned int n)
{
	char buf[20];
	char comm_path[40];
	pid_t tid;
	struct tcase *tc = &tcases[n];
	int ret = prctl(PR_SET_NAME, tc->setname);
	if (ret == -1) {
		printf("prctl(PR_SET_NAME) failed, error number %d\n", errno);
		return 0;
	}
	printf("prctl(PR_SET_NAME, '%s') succeeded\n", tc->setname);
	ret = prctl(PR_GET_NAME, buf);
	if (ret == -1) {
		printf("prctl(PR_GET_NAME) failed, error number %d\n", errno);
		return 0;
	}
	if (strncmp(tc->expname, buf, sizeof(buf))) {
		printf("prctl(PR_GET_NAME) failed, expected %s, got %s\n",tc->expname, buf);
		return 0;
	}
	printf("prctl(PR_GET_NAME) succeeded, thread name is %s\n", buf);
	return 1;
}

void main(){
	int ok=1;
	for(int i=0; i<2;i++)
		if(verify_prctl(i) == 0)
			ok=0;
	if(ok)
		printf("test succeeded\n");
}
