#include "incl.h"
#ifdef HAVE_SYS_CAPABILITY_H
# include <sys/capability.h>
#endif
#define PROC_STATUS "/proc/self/status"
#define ZERO_STRING "0000000000000000"
#define CAP_STRING  "0000000000000400"

inline void check_cap_raise(unsigned int cap, char *message, int fail_flag)
{
	int ret = prctl(PR_CAP_AMBIENT, PR_CAP_AMBIENT_RAISE, cap, 0, 0);
	switch (fail_flag) {
	case 0:
	if (ret == 0)
		printf("PR_CAP_AMBIENT_RAISE %s succeeded\n", message);
	else
		printf("PR_CAP_AMBIENT_RAISE %s failed unexpectedly\n", message);
	break;
	case 1:
	if (ret == 0)
		printf("PR_CAP_AMBIENT_RAISE succeeded unexpectedly %s\n",message);
	else if (errno == EPERM)
		printf("PR_CAP_AMBIENT_RAISE failed with EPERM %s\n", message);
	else
		printf("PR_CAP_AMBIENT_RAISE failed %s\n", message);
	break;
	}
}

inline void check_cap_is_set(unsigned int cap, char *message, int val)
{
	int ret = prctl(PR_CAP_AMBIENT, PR_CAP_AMBIENT_IS_SET, cap, 0, 0);
	if (ret == 1)
		printf("PR_CAP_AMBIENT_IS_SET %s in AmbientCap\n", message);
	else if (ret == 0)
		printf("PR_CAP_AMBIENT_IS_SET %s not in AmbientCap\n", message);
	else
		printf("PR_CAP_AMBIENT_IS_SET failed, error number %d\n", errno);
}

inline void check_cap_lower(unsigned int cap, char *message)
{
	int ret = prctl(PR_CAP_AMBIENT, PR_CAP_AMBIENT_LOWER, cap, 0, 0);
	if (ret == -1)
		printf("PR_CAP_AMBIENT_LOWER %s failed\n", message);
	else
		printf("PR_CAP_AMBIENT_LOWER %s succeeded\n", message);
}

int  verify_prctl()
{
#ifdef HAVE_LIBCAP
	cap_t caps = cap_init();
	cap_value_t caplist[3] = {CAP_NET_RAW, CAP_NET_BIND_SERVICE, CAP_SETPCAP};
	unsigned int numcaps = 3;
	cap_set_flag(caps, CAP_EFFECTIVE, numcaps, caplist, CAP_SET);
	cap_set_flag(caps, CAP_INHERITABLE, numcaps, caplist, CAP_SET);
	cap_set_flag(caps, CAP_PERMITTED, numcaps, caplist, CAP_SET);
	cap_set_proc(caps);
	printf("[I] At the beginning\n");
	cap_clear_flag(caps, CAP_INHERITABLE);
	cap_set_proc(caps);
	check_cap_raise(CAP_NET_BIND_SERVICE, "on non-inheritable cap", 1);
	cap_set_flag(caps, CAP_INHERITABLE, numcaps, caplist, CAP_SET);
	cap_clear_flag(caps, CAP_PERMITTED);
	cap_set_proc(caps);
	check_cap_raise(CAP_NET_RAW, "on non-permitted cap", 1);
	cap_set_flag(caps, CAP_PERMITTED, numcaps, caplist, CAP_SET);
	cap_set_proc(caps);
	prctl(PR_SET_SECUREBITS, SECBIT_NO_CAP_AMBIENT_RAISE);
	check_cap_raise(CAP_NET_BIND_SERVICE, "because of NO_RAISE_SECBIT set", 1);
	prctl(PR_SET_SECUREBITS, 0);
	check_cap_raise(CAP_NET_BIND_SERVICE, "CAP_NET_BIND_SERVICE", 0);
	check_cap_raise(CAP_NET_BIND_SERVICE, "CAP_NET_BIND_SERIVCE twice", 0);
	printf("[I] After PR_CAP_AMBIENT_RAISE\n");
	check_cap_is_set(CAP_NET_BIND_SERVICE, "CAP_NET_BIND_SERVICE was", 1);
	check_cap_is_set(CAP_NET_RAW, "CAP_NET_RAW was", 0);
	check_cap_lower(CAP_NET_RAW, "CAP_NET_RAW(it wasn't in ambient set)");
	check_cap_lower(CAP_NET_BIND_SERVICE, "CAP_NET_BIND_SERVICE(it was in ambient set)");
	printf("[I] After PR_CAP_AMBIENT_LORWER\n");
	prctl(PR_CAP_AMBIENT, PR_CAP_AMBIENT_RAISE, CAP_NET_BIND_SERVICE, 0, 0);
	printf("[I] raise cap for clear\n");
	int ret = prctl(PR_CAP_AMBIENT, PR_CAP_AMBIENT_CLEAR_ALL, 0, 0, 0);
	if (ret == 0)
		printf("PR_CAP_AMBIENT_CLEAR ALL succeeded\n");
	else
		printf("PR_AMBIENT_CLEAR_ALL failed, error number %d", errno);
	printf("[I] After PR_CAP_AMBIENT_CLEAR_ALL\n");
	cap_free(caps);
	return (!ret);
#else
	printf("libcap devel files missing during compilation\n");
	return 0;
#endif
}

void setup(void)
{
	int ret = prctl(PR_CAP_AMBIENT, PR_CAP_AMBIENT_CLEAR_ALL, 0, 0, 0);
	if (ret == 0) {
		printf("[I] kernel supports PR_CAP_AMBIENT\n");
		return;
	}
	if (errno == EINVAL)
		printf("kernel doesn't support PR_CAP_AMBIENT\n");
	printf("current environment doesn't permit PR_CAP_AMBIENT, error number %d", errno);
}

void main(){
	setup();
	if(verify_prctl())
		printf("test succeeded\n");
}
