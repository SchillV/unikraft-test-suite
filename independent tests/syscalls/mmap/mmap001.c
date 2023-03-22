#include "incl.h"
char *TCID = "mmap001";
int TST_TOTAL = 5;

char *filename = NULL;

int m_opt = 0;

char *m_copt;

void cleanup(void)
{
	free(filename);
}

void setup(void)
{
	char buf[1024];
	snprintf(buf, 1024, "testfile.%d", getpid());
	if ((filename = strdup(buf)) == NULL) {
		printf("strdup failed\n");
		cleanup();
		exit(EXIT_FAILURE);
	}
}

int main(int argc, char *argv[])
{
	char *array;
	int lc;
	unsigned int i;
	int fd;
	unsigned int pages, memsize;
	if (m_opt) {
		memsize = pages = atoi(m_copt);
		if (memsize < 1) {
			printf("Invalid arg for -m: %s\n", m_copt);
			exit(EXIT_FAILURE);
		}
	} else {
		memsize = pages = 1000;
		memsize *= getpagesize();
	}
	setup();
	fd = open(filename, O_RDWR | O_CREAT, 0666);
	if ((fd == -1)){
		printf("opening %s failed\n", filename);
		cleanup();
		exit(EXIT_FAILURE);
	}
	if (lseek(fd, memsize, SEEK_SET) != memsize) {
		close(fd);
		printf("lseek failed, error number %d\n", errno);
		exit(EXIT_FAILURE);
	}
	if (write(fd, "\0", 1) != 1) {
		close(fd);
		printf("writing to %s failed, error number %d\n", filename);
		exit(EXIT_FAILURE);
	}
	array = mmap(0, memsize, PROT_WRITE, MAP_SHARED, fd, 0);
	if (array == MAP_FAILED) {
		close(fd);
		printf("mmapping %s failed, error number %d\n", filename, errno);
		exit(EXIT_FAILURE);
	} else {
		printf("mmap() completed successfully.\n");
	}
	printf("[I] touching mmaped memory\n");
	for (i = 0; i < memsize; i++) {
		array[i] = (char)i;
	}
	printf("we're still here, mmaped area must be good\n");
	int ret = msync(array, memsize, MS_SYNC);
	if (ret == -1) {
		printf("synchronizing mmapped page failed, error number %d\n", errno);
		exit(EXIT_FAILURE);
	} else {
		printf("synchronizing mmapped page passed\n");
	}
	ret = munmap(array, memsize);
	if (ret == -1) {
		printf("munmapping %s failed, error number %d\n", filename, errno);
		exit(EXIT_FAILURE);
	} else {
		printf("munmapping %s successful\ntest succeeded\n", filename);
	}
	close(fd);
	unlink(filename);
	cleanup();
}

