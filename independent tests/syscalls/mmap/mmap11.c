#include "incl.h"
char *TCID = "mmap11";
int TST_TOTAL = 1;
#define MAL_SIZE (100*1024)

void *wait_thread(void *args);

void *wait_thread2(void *args);

void setup(void);

void cleanup(void);

void check(void);
int main(int argc, char *argv[])
{
	setup();
	check();
	cleanup();
}
void setup(void)
{
}
void cleanup(void)
{
}
void check(void)
{
	int lc;
	pthread_t *thread, th;
	int ret, count = 0;
	pthread_attr_t attr;
	ret = pthread_attr_init(&attr);
	if (ret){
		printf("pthread_attr_init, error number %d\n", errno);
		cleanup();
		exit(EXIT_FAILURE);
	}
	ret = pthread_attr_setdetachstate(&attr, PTHREAD_CREATE_DETACHED);
	if (ret){
		printf("pthread_attr_setdetachstate, error number %d\n", errno);
		cleanup();
		exit(EXIT_FAILURE);
	}
	thread = malloc(sizeof(pthread_t));
	if (thread == NULL){
		printf("malloc failed, error number %d\n", errno);
		cleanup();
		exit(EXIT_FAILURE);
	}
		ret = pthread_create(&th, &attr, wait_thread, NULL);
		if (ret) {
			printf("pthread_create failed, error number %d\n", errno);
			cleanup();
			exit(EXIT_FAILURE);
		}
		ret = pthread_create(&thread[lc], &attr, wait_thread2, NULL);
		if (ret) {
			printf("pthread_create failed, error number %d", errno);
			cleanup();
			exit(EXIT_FAILURE);
		}
	printf("test succeeded\n");
	free(thread);
}
void *wait_thread(void *args)
{
	void *addr;
	addr = malloc(MAL_SIZE);
	if (addr)
		memset(addr, 1, MAL_SIZE);
	sleep(1);
	return NULL;
}
void *wait_thread2(void *args)
{
	return NULL;
}

