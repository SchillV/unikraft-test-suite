#include "incl.h"

int  verify_prctl()
{
	int get_sig = 0;
	int ret = prctl(PR_SET_PDEATHSIG, SIGUSR2);
	if (ret == -1) {
		printf("prctl(PR_SET_PDEATHSIG) failed, error number %d\n", errno);
		return 0;
	}
	printf("prctl(PR_SET_PDEATHSIG) succeeded\n");
	ret = prctl(PR_GET_PDEATHSIG, &get_sig);
	if (ret == -1) {
		printf("prctl(PR_GET_PDEATHSIG) failed, error number %d\n", errno);
		return 0;
	}
	if (get_sig == SIGUSR2) {
		printf("prctl(PR_GET_PDEATHSIG) got expected death signal\n");
		return 1;
	} else {
		printf("prctl(PR_GET_PDEATHSIG) got death signal %d, expected %d", get_sig, SIGUSR2);
		return 0;
	}
}

void main(){
	if(verify_prctl())
		printf("test succeeded\n");
}
