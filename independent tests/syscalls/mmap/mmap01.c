#include "incl.h"
#define TEMPFILE	"mmapfile"
char *TCID = "mmap01";
int TST_TOTAL = 1;

char *addr;

char *dummy;

size_t page_sz;

size_t file_sz;

int fildes;

char cmd_buffer[BUFSIZ];

void setup(void);
void cleanup(void);

int main(int ac, char **av)
{
	setup();
	errno = 0;
	addr = mmap(NULL, page_sz, PROT_READ | PROT_WRITE, MAP_FILE | MAP_SHARED, fildes, 0);
	if (addr == MAP_FAILED) {
		printf("mmap of %s failed\n", TEMPFILE);
		exit(EXIT_FAILURE);
	}
	if (memcmp(&addr[file_sz], dummy, page_sz - file_sz)) {
		printf("mapped memory area contains invalid data\n");
		cleanup();
		exit(EXIT_FAILURE);
	}
	addr[file_sz] = 'X';
	addr[file_sz + 1] = 'Y';
	addr[file_sz + 2] = 'Z';
	if (msync(addr, page_sz, MS_SYNC) != 0) {
		printf("failed to synchronize mapped file\n");
		exit(EXIT_FAILURE);
	}
	if (system(cmd_buffer) != 0) {
		printf("test succeded\n");
	} else {
		printf("Specified pattern found in file\n");
	}
	cleanup();
}

void setup(void)
{
	struct stat stat_buf;
	char Path_name[1024];
	char write_buf[] = "hello world\n";
	if (getcwd(Path_name, sizeof(Path_name)) == NULL) {
		printf("getcwd failed to get current working directory\n");
		cleanup();
		exit(EXIT_FAILURE);
	}
	if ((fildes = open(TEMPFILE, O_RDWR | O_CREAT, 0666)) < 0) {
		printf("opening %s failed\ntest failed\n", TEMPFILE);
		cleanup();
		exit(EXIT_FAILURE);
	}
	if (write(fildes, write_buf, strlen(write_buf)) != (long)strlen(write_buf)) {
		printf("writing to %s\ntest failed\n", TEMPFILE);
		cleanup();
		exit(EXIT_FAILURE);
	}
	if (stat(TEMPFILE, &stat_buf) < 0) {
		printf("stat of %s failed\ntest failed\n", TEMPFILE);
		cleanup();
		exit(EXIT_FAILURE);
	}
	file_sz = stat_buf.st_size;
	page_sz = getpagesize();
	if ((dummy = calloc(page_sz, sizeof(char))) == NULL) {
		printf("calloc failed (dummy)\ntest failed\n");
		cleanup();
		exit(EXIT_FAILURE);
	}
	sprintf(cmd_buffer, "grep XYZ %s/%s > /dev/null", Path_name, TEMPFILE);
}

void cleanup(void)
{
	close(fildes);
	free(dummy);
}

