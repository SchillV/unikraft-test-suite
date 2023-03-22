#include "prctl06.h"

uid_t nobody_uid;
gid_t nobody_gid;
int proc_flag = 1;
char *proc_sup = "Yes";

int cp(const char *to, const char *from)
{
    int fd_to, fd_from;
    char buf[4096];
    ssize_t nread;
    int saved_errno;

    fd_from = open(from, O_RDONLY);
    if (fd_from < 0)
        return -1;

    fd_to = open(to, O_WRONLY | O_CREAT | O_EXCL, 0666);
    if (fd_to < 0)
        goto out_error;

    while (nread = read(fd_from, buf, sizeof buf), nread > 0)
    {
        char *out_ptr = buf;
        ssize_t nwritten;

        do {
            nwritten = write(fd_to, out_ptr, nread);

            if (nwritten >= 0)
            {
                nread -= nwritten;
                out_ptr += nwritten;
            }
            else if (errno != EINTR)
            {
                goto out_error;
            }
        } while (nread > 0);
    }

    if (nread == 0)
    {
        if (close(fd_to) < 0)
        {
            fd_to = -1;
            goto out_error;
        }
        close(fd_from);

        /* Success! */
        return 0;
    }

  out_error:
    saved_errno = errno;

    close(fd_from);
    if (fd_to >= 0)
        close(fd_to);

    errno = saved_errno;
    return -1;
}

void do_prctl(void)
{
	char ipc_env_var[1024];
	char *const argv[] = {BIN_PATH, "After execve, parent process", proc_sup, NULL};
	char *const childargv[] = {BIN_PATH, "After execve, child process", proc_sup, NULL};
	char *const envp[] = {ipc_env_var, NULL };
	int childpid;
	check_no_new_privs(0, "parent", proc_flag);
	int ret = prctl(PR_SET_NO_NEW_PRIVS, 1, 0, 0, 0);
	if (ret == -1) {
		printf("prctl(PR_SET_NO_NEW_PRIVS) failed, error number %d\n", errno);
		return;
	}
	printf("prctl(PR_SET_NO_NEW_PRIVS) succeeded\n");
	setgid(nobody_gid);
	setuid(nobody_uid);
	childpid = fork();
	if (childpid == 0) {
		check_no_new_privs(1, "After fork, child process", proc_flag);
		execve(BIN_PATH, childargv, envp);
		printf("child process failed to execute prctl_execve, error number %d\n", errno);
	} else {
		check_no_new_privs(1, "parent process", proc_flag);
		execve(BIN_PATH, argv, envp);
		printf("parent process failed to execute prctl_execve, error number %d\n", errno);
	}
}

int verify_prctl()
{
	int pid;
	pid = fork();
	if (pid == 0) {
		do_prctl();
		exit(0);
	}
}

void setup(void)
{
	struct passwd *pw;
	int field;
	pw = getpwnam("nobody");
	nobody_uid = pw->pw_uid;
	nobody_gid = pw->pw_gid;
	cp(TESTBIN, TEST_REL_BIN_DIR);
	chown(BIN_PATH, 0, 0);
	chmod(BIN_PATH, SUID_MODE);
	if (fscanf(PROC_STATUS, "NoNewPrivs:%d", &field)) {
		printf("%s doesn't support NoNewPrivs field\n", PROC_STATUS);
		proc_flag = 0;
		proc_sup = "No";
	}
	int ret = prctl(PR_GET_NO_NEW_PRIVS, 0, 0, 0, 0);
	if (ret == 0) {
		printf("[I] kernel supports PR_GET/SET_NO_NEW_PRIVS\n");
		return;
	}
	if (errno == EINVAL)
		printf("kernel doesn't support PR_GET/SET_NO_NEW_PRIVS\n");
	printf("current environment doesn't permit PR_GET/SET_NO_NEW_PRIVS\n");
	exit(0);
}

void main(){
	setup();
	
}
