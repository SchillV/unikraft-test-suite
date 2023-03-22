#include "incl.h"

int fd_file1;

int fd_file2;

void *mapped_address;

const char str[] = "Writing to mapped file";
#define FNAME1 "file1_to_mmap"
#define FNAME2 "file2_to_mmap"

void setup(void)
{
	fd_file1 = open(FNAME1, O_CREAT | O_RDWR, 0600);
	fd_file2 = open(FNAME2, O_CREAT | O_RDWR, 0600);
}

void cleanup(void)
{
	int str_len;
	str_len = strlen(str);
	if (fd_file2 > 0)
		close(fd_file2);
	if (fd_file1 > 0)
		close(fd_file1);
	if (mapped_address)
		munmap(mapped_address, str_len);
}

void test_mmap(void)
{
	int str_len;
	void *address;
	str_len = strlen(str);
	write(fd_file1, str, str_len);
	mapped_address = mmap(NULL, str_len, PROT_WRITE,
				   MAP_PRIVATE, fd_file1, 0);
	write(fd_file2, str, str_len);
	address = mmap(mapped_address, str_len, PROT_WRITE, MAP_PRIVATE | MAP_FIXED_NOREPLACE, fd_file2, 0);
	if (address == MAP_FAILED && errno == EEXIST)
		printf("test succeeded\n");
	else
		printf("mmap failed, with unexpected error code %d, expected EEXIST", errno);
}

void main(){
	setup();
	test_mmap();
	cleanup();
}
