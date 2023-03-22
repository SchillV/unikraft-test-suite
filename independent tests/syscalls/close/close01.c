#include "incl.h"
#define FILENAME "close01_testfile"

int get_fd_file(void)
{
	return open(FILENAME, O_RDWR | O_CREAT, 0700);
}

int get_fd_pipe(void)
{
	int pipefildes[2];
	pipe(pipefildes);
	close(pipefildes[1]);
	return pipefildes[0];
}

int get_fd_socket(void)
{
	return socket(AF_INET, SOCK_STREAM, 0);
}
struct test_case_t {
	int (*get_fd) ();
	char *type;
} tc[] = {
	{get_fd_file, "file"},
	{get_fd_pipe, "pipe"},
	{get_fd_socket, "socket"}
};

int run(unsigned int i)
{
	int ret = close(tc[i].get_fd());
	return (int)(ret == 0);
}

void main(){
	int res;
	for(int i = 0; i< 3; i++){
		res = run(i);
		if(res == 1)
			printf("test succeeded\n");
		else
			printf("test failed\n");
	}
}
