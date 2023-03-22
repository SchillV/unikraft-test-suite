#include "incl.h"
#define FNAME "filename"

static const struct sock_filter  strict_filter[] = {
	BPF_STMT(BPF_LD | BPF_W | BPF_ABS, (offsetof(struct seccomp_data, nr))),
	BPF_JUMP(BPF_JMP | BPF_JEQ, __NR_rt_sigprocmask, 6, 0),
	BPF_JUMP(BPF_JMP | BPF_JEQ, __NR_close, 5, 0),
	BPF_JUMP(BPF_JMP | BPF_JEQ, __NR_exit,  4, 0),
	BPF_JUMP(BPF_JMP | BPF_JEQ, __NR_wait4, 3, 0),
	BPF_JUMP(BPF_JMP | BPF_JEQ, __NR_write, 2, 0),
	BPF_JUMP(BPF_JMP | BPF_JEQ, __NR_clone, 1, 0),
	BPF_STMT(BPF_RET | BPF_K, SECCOMP_RET_KILL),
	BPF_STMT(BPF_RET | BPF_K, SECCOMP_RET_ALLOW)
};

static const struct sock_fprog  strict = {
	.len = (unsigned short)(sizeof(strict_filter)/sizeof(strict_filter[0])),
	.filter = (struct sock_filter *)strict_filter
};

void check_strict_mode(int);

void check_filter_mode(int);

struct tcase {
	void (*func_check)();
	int pass_flag;
	int val;
	int exp_signal;
	char *message;
} tcases[] = {
	{check_strict_mode, 1, 1, SIGKILL,
	"SECCOMP_MODE_STRICT doesn't permit GET_SECCOMP call"},
	{check_strict_mode, 0, 2, SIGKILL,
	"SECCOMP_MODE_STRICT doesn't permit read(2) write(2) and _exit(2)"},
	{check_strict_mode, 1, 3, SIGKILL,
	"SECCOMP_MODE_STRICT doesn't permit close(2)"},
	{check_filter_mode, 1, 1, SIGSYS,
	"SECCOMP_MODE_FILTER doestn't permit GET_SECCOMP call"},
	{check_filter_mode, 0, 2, SIGSYS,
	"SECCOMP_MODE_FILTER doesn't permit close(2)"},
	{check_filter_mode, 2, 3, SIGSYS,
	"SECCOMP_MODE_FILTER doesn't permit exit()"},
	{check_filter_mode, 0, 4, SIGSYS,
	"SECCOMP_MODE_FILTER doesn't permit exit()"}
};

int mode_filter_not_supported;

void check_filter_mode_inherit(void)
{
	int childpid;
	int childstatus;
	childpid = fork();
	if (childpid == 0) {
		printf("SECCOMP_MODE_FILTER permits fork(2)\n");
		exit(0);
	}
	wait4(childpid, &childstatus, 0, NULL);
	if (WIFSIGNALED(childstatus) && WTERMSIG(childstatus) == SIGSYS)
		printf("SECCOMP_MODE_FILTER has been inherited by child\n");
	else
		printf("SECCOMP_MODE_FILTER isn't been inherited by child\n");
}

void check_strict_mode(int val)
{
	int fd;
	char buf[2];
	fd = open(FNAME, O_RDWR | O_CREAT, 0666);
	int ret = prctl(PR_SET_SECCOMP, SECCOMP_MODE_STRICT);
	if (ret == -1) {
		printf("prctl(PR_SET_SECCOMP) sets SECCOMP_MODE_STRICT failed, error number %d\n", errno);
		return;
	}
	switch (val) {
	case 1:
		printf("prctl(PR_SET_SECCOMP) sets SECCOMP_MODE_STRICT succeed\n");
		prctl(PR_GET_SECCOMP);
		printf("prctl(PR_GET_SECCOMP) succeed unexpectedly\n");
	break;
	case 2:
		write(fd, "a", 1);
		read(fd, buf, 1);
		printf("SECCOMP_MODE_STRICT permits read(2) write(2) and _exit(2)\n");
	break;
	case 3:
		close(fd);
		printf("SECCOMP_MODE_STRICT permits close(2) unexpectdly\n");
	break;
	}
}

void check_filter_mode(int val)
{
	int fd;
	if (mode_filter_not_supported == 1) {
		printf("kernel doesn't support SECCOMP_MODE_FILTER\n");
		return;
	}
	fd = open(FNAME, O_RDWR | O_CREAT, 0666);
	int ret = prctl(PR_SET_SECCOMP, SECCOMP_MODE_FILTER, &strict);
	if (ret == -1) {
		printf("prctl(PR_SET_SECCOMP) sets SECCOMP_MODE_FILTER failed, error number %d\n", errno);
		return;
	}
	switch (val) {
	case 1:
		printf("prctl(PR_SET_SECCOMP) sets SECCOMP_MODE_FILTER succeed\n");
		prctl(PR_GET_SECCOMP);
		printf("prctl(PR_GET_SECCOMP) succeed unexpectedly\n");
	break;
	case 2:
		close(fd);
		printf("SECCOMP_MODE_FILTER permits close(2)\n");
	break;
	case 3:
		exit(0);
	break;
	case 4:
		check_filter_mode_inherit();
	break;
	}
}

int verify_prctl(unsigned int n)
{
	int pid;
	int status;
	struct tcase *tc = &tcases[n];
	pid = fork();
	if (pid == 0) {
		tc->func_check(tc->val);
	} else {
		waitpid(pid, &status, 0);
		if (WIFSIGNALED(status) && WTERMSIG(status) == tc->exp_signal) {
			if (tc->pass_flag){
				printf("%s\n", tc->message);
				return 1;
			}
			else{
				printf("%s\n", tc->message);
				return 0;
			}
		}
		if (tc->pass_flag == 2 && mode_filter_not_supported == 0){
			printf("SECCOMP_MODE_FILTER permits exit() unexpectedly\n");
			return 0;
		}
	}
}

void setup(void)
{
	int ret = prctl(PR_GET_SECCOMP);
	if (ret == 0) {
		printf("[I] kernel supports PR_GET/SET_SECCOMP\n");
		ret = prctl(PR_SET_SECCOMP, SECCOMP_MODE_FILTER, NULL);
		if (ret == -1 && errno == EINVAL) {
			mode_filter_not_supported = 1;
			return;
		}
		printf("[I] kernel supports SECCOMP_MODE_FILTER\n");
		return;
	}
	if (errno == EINVAL)
		printf("kernel doesn't support PR_GET/SET_SECCOMP\n");
	printf("current environment doesn't permit PR_GET/SET_SECCOMP, error number %d\n", errno);
}

void main(){
	setup();
	int ok =1;
	for(int i=0;i<7;i++)
		if(verify_prctl(i) == 0)
			ok = 0;
	if(ok)
		printf("test succeeded\n");
}
