#include "incl.h"
#define TEMPFILE	"mmapfile"
char *TCID = "mmap13";
int TST_TOTAL = 1;
size_t page_sz;
char *addr;
int fildes;
static volatile sig_atomic_t pass;
void setup(void);
void cleanup(void);
static void sig_handler(int sig);

int main(int argc, char *argv[])
{
	char *ch;
	setup();
	addr = mmap(NULL, page_sz * 2, PROT_READ | PROT_WRITE, MAP_FILE | MAP_SHARED, fildes, 0);
	sig_handler(SIGBUS);
	if (addr == MAP_FAILED) {
		printf("mmap() failed on %s, error number %d\n", TEMPFILE, errno);
		exit(0);
	}
	if (pass){
		printf("test succeeded\n");
	}
	else{
		printf("Invalid access not rise SIGBUS\n");
	}
	if (munmap(addr, page_sz * 2) != 0){
		printf("failed to unmap the mmapped pages, error number %d\n",errno);
		cleanup();
	}
	cleanup();
}

void setup(void)
{
	page_sz = getpagesize();
	fildes = open(TEMPFILE, O_RDWR | O_CREAT, 0766);
	if (fildes < 0){
		printf("opening %s failed, error number %d\n", TEMPFILE, errno);
		cleanup();
		exit(0);
	}
	if (ftruncate(fildes, page_sz / 2) == -1){
		printf("ftruncate %s failed, error number %d\n", TEMPFILE, errno);
		cleanup();
		exit(0);
	}
}

static void sig_handler(int sig)
{
	if (sig == SIGBUS) {
		pass = 1;
	} else {
		printf("received an unexpected signal\n");
		cleanup();
		exit(0);
	}
}

void cleanup(void)
{
	close(fildes);
}

