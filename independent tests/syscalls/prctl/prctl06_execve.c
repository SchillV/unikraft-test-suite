#include "prctl06.h"
#define TST_NO_DEFAULT_MAIN
int main(int argc, char **argv)
{
	struct passwd *pw;
	int proc_flag, ok=1;
	pw = getpwnam("nobody");
	if (argc != 3){
		printf("argc is %d, expected 3\n", argc);
		ok=0;
	}
	if (!strcmp(argv[2], "Yes"))
		proc_flag = 1;
	else
		proc_flag = 0;
	check_no_new_privs(1, argv[1], proc_flag);
	int ret = getegid();
	if (ret == 0){
		printf("%s getegid() returns 0 unexpectedly, it gains root privileges\n", argv[1]);
		ok=0;
	}
	if (ret == pw->pw_gid)
		printf("%s getegid() returns nobody, it doesn't gain root privileges\n",argv[1]);
	ret = geteuid();
	if (ret == 0){
		printf("%s geteuid() returns 0 unexpectedly, it gains root privileges\n",argv[1]);
		ok=0;
	}
	if (ret == pw->pw_uid)
		printf("%s geteuid() returns nobody, it doesn't gain root privileges\n",argv[1]);
	if(ok)
		printf("test succeeded\n");
	return 0;
}

