#include "incl.h"
#define _GNU_SOURCE
#define TEST_APP "openat02_child"
#define TEST_FILE "test_file"
#define SFILE "sfile"
#define LARGE_FILE "large_file"
#define STR "abcdefg"

void setup(void);

void cleanup(void);

int testfunc_append(void);

int testfunc_cloexec(void);

int testfunc_largefile(void);

int testfunc_noatime(void);

int testfunc_nofollow(void);

int testfunc_trunc(void);

int (*testfunc[])(void) = {
	testfunc_append,
	testfunc_cloexec,
	testfunc_largefile,
	testfunc_noatime,
	testfunc_nofollow,
	testfunc_trunc,
};
char *TCID = "openat02";
int TST_TOTAL = sizeof(testfunc)/sizeof(testfunc[0]);
int main(int ac, char **av)
{
	int lc;
	int i, ok=1;
	setup();
	for (i = 0; i < TST_TOTAL; i++)
		ok *= (*testfunc[i])();
	cleanup();
	if(ok)
		printf("test succeeded\n");
}
void setup(void)
{
	fprintf(TEST_FILE, "test file");
	symlink(TEST_FILE, SFILE);
}
int testfunc_append(void)
{
	off_t file_offset;
	fprintf(TEST_FILE, "test file");
	int ret = openat(AT_FDCWD, TEST_FILE, O_APPEND | O_RDWR, 0777);
	if (ret == -1) {
		printf("openat failed\n");
		return 0;
	}
	write(ret, STR, sizeof(STR) - 1);
	file_offset = lseek(ret, 0, SEEK_CUR);
	if (file_offset > (off_t)(sizeof(STR) - 1)){
		printf("test O_APPEND for openat success\n");
		return 1;
	} else
		printf("test O_APPEND for openat failed\n");
	close(ret);
	return 0;
}
int testfunc_cloexec(void)
{
	pid_t pid;
	int status;
	char buf[20];
	int ret = openat(AT_FDCWD, TEST_FILE, __O_CLOEXEC | O_RDWR, 0777);
	if (ret == -1) {
		printf("openat failed, error number %d\n", errno);
		return 0;
	}
	sprintf(buf, "%ld", ret);
	if (pid == 0) {
		if (execlp(TEST_APP, TEST_APP, buf, NULL))
			exit(2);
	}
	close(ret);
	wait(&status);
	if (WIFEXITED(status)) {
		switch ((int8_t)WEXITSTATUS(status)) {
		case 0:
			printf("test O_CLOEXEC for openat success\n");
			return 1;
		break;
		case 1:
			printf("test O_CLOEXEC for openat failed\n");
			return 0;
		break;
		default:
			printf("execlp() failed\n");
			return 0;
		}
	} else {
		printf("openat2_exec exits with unexpected error, got %d\n", errno);
		cleanup();
		return 0;
	}
}
int testfunc_largefile(void)
{
	int fd;
	off_t offset;
	fd = open(LARGE_FILE, __O_LARGEFILE | O_RDWR | O_CREAT, 0777);
	offset = lseek(fd, 4.1*1024*1024*1024, SEEK_SET);
	if (offset == -1){
		printf("lseek64 failed, error number %d\n", errno);
		cleanup();
		return 0;
	}
	write(fd, STR, sizeof(STR) - 1);
	close(fd);
	int ret = openat(AT_FDCWD, LARGE_FILE, __O_LARGEFILE | O_RDONLY, 0777);
	if (ret == -1) {
		printf("test O_LARGEFILE for openat failed\n");
		return 0;
	} else {
		printf("test O_LARGEFILE for openat success\n");
		close(ret);
		return 1;
	}
}
int testfunc_noatime(void)
{
	struct stat file_stat, file_newstat;
	char buf;
	const char *flags[] = {"noatime", "relatime", NULL};
	int ret;
	stat(TEST_FILE, &file_stat);
	sleep(1);
	ret = openat(AT_FDCWD, TEST_FILE, __O_NOATIME | O_RDONLY, 0777);
	if (ret == -1) {
		printf("openat failed, error number %d\n", errno);
		return 0;
	}
	read(ret, &buf, 1);
	close( ret);
	stat(TEST_FILE, &file_newstat);
	if (file_stat.st_atime == file_newstat.st_atime){
		printf("test O_NOATIME for openat success\n");
		return 1;
	} else {
		printf("test O_NOATIME for openat failed\n");
		return 0;
	}
}
int testfunc_nofollow(void)
{
	int ret = openat(AT_FDCWD, SFILE, __O_NOFOLLOW | O_RDONLY, 0777);
	if (ret == -1 && errno == ELOOP) {
		printf("test O_NOFOLLOW for openat success\n");
		return 1;
	} else {
		printf("test O_NOFOLLOW for openat failed\n");
		close(ret);
		return 0;
	}
}
int testfunc_trunc(void)
{
	struct stat file_stat;
	int ret = openat(AT_FDCWD, TEST_FILE, O_TRUNC | O_RDWR, 0777);
	if (ret == -1) {
		printf("openat failed, error number %d\n", errno);
		return 0;
	}
	fstat(ret, &file_stat);
	if (file_stat.st_size == 0){
		printf("test O_TRUNC for openat success\n");
	} else {
		printf("test O_TRUNC for openat failed\n");
	}
	close(ret);
	return (file_stat.st_size == 0);
}
void cleanup(void)
{
}

