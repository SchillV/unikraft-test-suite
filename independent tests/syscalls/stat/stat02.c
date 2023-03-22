#include "incl.h"
#define FNAME1	"stat02.1"
#define FNAME2	"stat02.2"
#define NUM_WRITES	(10)
#define BUFSIZE		(4096)
char *buffer;

struct test_case {
	const char *filename;
	size_t bytes_to_write;
	size_t decrement;
} tcases[] = {
	{ FNAME1, BUFSIZE, BUFSIZE },
	{ FNAME2, BUFSIZE, 1000 }
};
int  verify(const char *fname, size_t bytes, size_t decrement)
{
	struct stat s;
	int fd, i;
	size_t bytes_written = 0;
	fd = open(fname, O_CREAT | O_TRUNC | O_RDWR, 0777);
	while (bytes > 0) {
		for (i = 0; i < NUM_WRITES; i++) {
			write(1, fd, buffer, bytes);
			bytes_written += bytes;
		}
		bytes -= bytes > decrement ? decrement : bytes;
	}
	close(fd);
	stat(fname, &s);
	unlink(fname);
	 *  Now check to see if the number of bytes written was
	 *  the same as the number of bytes in the file.
	 */
	if (s.st_size != (off_t) bytes_written) {
		tst_res(TFAIL, "file size (%zu) not as expected (%zu) bytes",
				s.st_size, bytes_written);
		return;
	}
	tst_res(TPASS, "File size reported as expected");
}
int  verify_stat_size(unsigned int nr)
{
	struct test_case *tcase = &tcases[nr];
int 	verify(tcase->filename, tcase->bytes_to_write, tcase->decrement);
}
void setup(void)
{
	buffer = malloc(BUFSIZE);
}
void cleanup(void)
{
	free(buffer);
}

void main(){
	setup();
	cleanup();
}
