#include "incl.h"
char *TCID = "mprotect01";
int TST_TOTAL = 3;
struct test_case {
	void *addr;
	int len;
	int prot;
	int error;
	void (*setupfunc) (struct test_case *self);
};

void cleanup(void);

void setup(void);

void setup1(struct test_case *self);

void setup2(struct test_case *self);

void setup3(struct test_case *self);

int fd;
struct test_case TC[] = {
	{NULL, 0, PROT_READ, ENOMEM, setup1},
	{NULL, 1024, PROT_READ, EINVAL, setup2},
	{NULL, 0, PROT_WRITE, EACCES, setup3}
};
int main(int ac, char **av)
{
	int i, ret, ok = 1;
	setup();
	for (i = 0; i < TST_TOTAL; i++) {
		if (TC[i].setupfunc != NULL)
			TC[i].setupfunc(&TC[i]);
		ret = syscall(__NR_mprotect, TC[i].addr, TC[i].len, TC[i].prot);
		if (ret != -1) {
			printf("call succeeded unexpectedly\n");
			ok = 0;
			continue;
		}
		if (errno == TC[i].error) {
			printf("expected failure - errno = %d\n", errno);
		} else {
			printf("unexpected error - %d - expected %d\n", errno, TC[i].error);
			ok = 0;
		}
	}
	if(ok)
		printf("test succeeded\n");
	cleanup();
}

void setup1(struct test_case *self)
{
	self->len = getpagesize() + 1;
}

void setup2(struct test_case *self)
{
	self->addr = malloc(getpagesize());
	if (self->addr == NULL){
		printf("malloc failed\n");
		cleanup();
		exit(0);
	}
	self->addr++;
}

void setup3(struct test_case *self)
{
	fd = open("/dev/zero", O_RDONLY);
	self->len = getpagesize();
	self->addr = mmap(0, self->len, PROT_READ, MAP_SHARED, fd, 0);
	if (self->addr == MAP_FAILED){
		printf("mmap failed\n");
		cleanup();
		exit(0);
	}
}

void setup(void)
{
}

void cleanup(void)
{
	close(fd);
}

