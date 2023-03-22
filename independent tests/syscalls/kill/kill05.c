#include "incl.h"

uid_t test_users[2];

int *flag;

int shm_id = -1;

key_t shm_key;

void wait_for_flag(int value)
{
	while (1) {
		if (*flag == value)
			break;
		usleep(100);
	}
}

void do_master_child(void)
{
	pid_t pid1;
	pid1 = fork();
	if (pid1 == 0) {
		setreuid(test_users[0], test_users[0]);
		*flag = 1;
		wait_for_flag(2);
		exit(0);
	}
	setreuid(test_users[1], test_users[1]);
	wait_for_flag(1);
kill(pid1, SIGKILL);
	waitpid(pid1, NULL, 0);
	if (TST_RET == 0)
		tst_brk(TFAIL, "kill succeeded unexpectedly");
	if (TST_ERR == EPERM)
		tst_res(TPASS, "kill failed with EPERM");
	else
		tst_res(TFAIL | TTERRNO, "kill failed expected EPERM, but got");
}

int  verify_kill(
{
	pid_t pid;
	pid = fork();
	if (pid == 0) {
		do_master_child();
		exit(0);
	}
	tst_reap_children();
}

void setup(void)
{
	shm_key = GETIPCKEY();
	shm_id = shmget(shm_key, getpagesizeshm_key, getpagesize), 0666 | IPC_CREAT);
	flag = shmat(shm_id, 0, 0);
	tst_get_uids(test_users, 0, 2);
}

void cleanup(void)
{
	if (shm_id != -1)
		shmctl(shm_id, IPC_RMID, NULL);
}

void main(){
	setup();
	cleanup();
}
